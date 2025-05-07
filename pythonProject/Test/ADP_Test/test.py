import sys
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout,
                             QHBoxLayout, QLabel, QLineEdit, QPushButton,
                             QTextEdit, QComboBox, QSpinBox, QCheckBox)
from PyQt5.QtCore import QRegExp, QTimer
from PyQt5.QtGui import QRegExpValidator
from pymodbus.client import ModbusSerialClient, ModbusTcpClient


class HexLineEdit(QLineEdit):
    """支持十六进制输入的文本框"""

    def __init__(self, parent=None):
        super().__init__(parent)
        # 设置验证器，只允许输入0-9和a-f/A-F
        hex_validator = QRegExpValidator(QRegExp(r'^[0-9a-fA-F]+$'), self)
        self.setValidator(hex_validator)

    def value(self):
        """获取十六进制文本框的值（转换为十进制整数）"""
        text = self.text()
        return int(text, 16) if text else 0


class ModbusMasterGUI(QMainWindow):
    def __init__(self):
        super().__init__()
        self.client = None
        self.timer = QTimer()
        self.timer.timeout.connect(self.auto_read_registers)
        self.initUI()

    def initUI(self):
        self.setWindowTitle('Modbus上位机')
        self.setGeometry(100, 100, 800, 600)  # 增大窗口尺寸

        central_widget = QWidget()
        self.setCentralWidget(central_widget)

        layout = QVBoxLayout()

        # 连接设置
        conn_layout = QHBoxLayout()
        self.protocol_cb = QComboBox()
        self.protocol_cb.addItems(['RTU', 'TCP'])
        self.protocol_cb.currentTextChanged.connect(self.update_ui)

        self.port_edit = QLineEdit('COM36')
        self.baudrate_edit = QLineEdit('9600')
        self.ip_edit = QLineEdit('192.168.1.100')
        self.ip_edit.setVisible(False)
        self.port_label = QLabel('端口:')

        self.connect_btn = QPushButton('连接')
        self.connect_btn.clicked.connect(self.toggle_connection)

        conn_layout.addWidget(QLabel('协议:'))
        conn_layout.addWidget(self.protocol_cb)
        conn_layout.addWidget(self.port_label)
        conn_layout.addWidget(self.port_edit)
        conn_layout.addWidget(QLabel('波特率:'))
        conn_layout.addWidget(self.baudrate_edit)
        conn_layout.addWidget(self.ip_edit)
        conn_layout.addWidget(self.connect_btn)

        # 操作设置
        op_layout = QHBoxLayout()
        self.func_code_cb = QComboBox()
        self.func_code_cb.addItems(['读取线圈 (01)', '读取输入状态 (02)',
                                    '读取保持寄存器 (03)', '读取输入寄存器 (04)',
                                    '写入单个线圈 (05)', '写入单个寄存器 (06)',
                                    '写入多个线圈 (15)', '写入多个寄存器 (16)'])

        self.slave_id_spin = QSpinBox()
        self.slave_id_spin.setRange(1, 247)
        self.slave_id_spin.setValue(1)

        # 将地址输入框改为十六进制输入
        self.addr_edit = HexLineEdit()
        self.addr_edit.setPlaceholderText('十六进制地址')
        self.addr_edit.setText('0')  # 默认地址0

        self.count_spin = QSpinBox()
        self.count_spin.setRange(1, 125)
        self.count_spin.setValue(10)

        self.value_edit = QLineEdit()
        self.value_edit.setPlaceholderText('写入值(多个值用逗号分隔)')

        self.execute_btn = QPushButton('执行')
        self.execute_btn.clicked.connect(self.execute_command)

        op_layout.addWidget(QLabel('功能码:'))
        op_layout.addWidget(self.func_code_cb)
        op_layout.addWidget(QLabel('从站ID:'))
        op_layout.addWidget(self.slave_id_spin)
        op_layout.addWidget(QLabel('地址(HEX):'))
        op_layout.addWidget(self.addr_edit)
        op_layout.addWidget(QLabel('数量:'))
        op_layout.addWidget(self.count_spin)
        op_layout.addWidget(self.value_edit)
        op_layout.addWidget(self.execute_btn)

        # 自动读取设置
        auto_read_layout = QHBoxLayout()
        self.auto_read_check = QCheckBox('定时连续读取')
        self.auto_read_check.stateChanged.connect(self.toggle_auto_read)

        self.interval_spin = QSpinBox()
        self.interval_spin.setRange(100, 10000)  # 100ms到10s
        self.interval_spin.setValue(1000)  # 默认1秒
        self.interval_spin.setSuffix('ms')

        self.start_addr_edit = HexLineEdit()
        self.start_addr_edit.setPlaceholderText('起始地址')
        self.start_addr_edit.setText('0')

        self.read_count_spin = QSpinBox()
        self.read_count_spin.setRange(1, 125)
        self.read_count_spin.setValue(10)

        auto_read_layout.addWidget(self.auto_read_check)
        auto_read_layout.addWidget(QLabel('间隔:'))
        auto_read_layout.addWidget(self.interval_spin)
        auto_read_layout.addWidget(QLabel('起始地址:'))
        auto_read_layout.addWidget(self.start_addr_edit)
        auto_read_layout.addWidget(QLabel('读取数量:'))
        auto_read_layout.addWidget(self.read_count_spin)
        auto_read_layout.addStretch()

        # 日志输出
        self.log_edit = QTextEdit()
        self.log_edit.setReadOnly(True)

        layout.addLayout(conn_layout)
        layout.addLayout(op_layout)
        layout.addLayout(auto_read_layout)
        layout.addWidget(self.log_edit)

        central_widget.setLayout(layout)
        self.update_ui()

    def update_ui(self):
        is_rtu = self.protocol_cb.currentText() == 'RTU'
        self.port_edit.setVisible(is_rtu)
        self.baudrate_edit.setVisible(is_rtu)
        self.ip_edit.setVisible(not is_rtu)
        self.port_label.setText('COM端口:' if is_rtu else 'IP地址:')

    def toggle_connection(self):
        if self.client and self.client.is_socket_open():
            self.client.close()
            self.connect_btn.setText('连接')
            self.log('断开连接')
            self.timer.stop()
            self.auto_read_check.setChecked(False)
        else:
            protocol = self.protocol_cb.currentText()
            try:
                if protocol == 'RTU':
                    self.client = ModbusSerialClient(
                        port=self.port_edit.text(),
                        framer='rtu',
                        baudrate=int(self.baudrate_edit.text()),
                        timeout=1
                    )
                else:  # TCP
                    self.client = ModbusTcpClient(
                        host=self.ip_edit.text(),
                        port=502
                    )

                if self.client.connect():
                    self.connect_btn.setText('断开')
                    self.log(f'成功连接到 {self.port_edit.text() if protocol == "RTU" else self.ip_edit.text()}')
                else:
                    self.log('连接失败')
            except Exception as e:
                self.log(f'连接错误: {str(e)}')

    def toggle_auto_read(self, state):
        """切换定时自动读取状态"""
        if state:  # 复选框被选中
            if not self.client or not self.client.is_socket_open():
                self.log('错误: 未连接到设备')
                self.auto_read_check.setChecked(False)
                return

            interval = self.interval_spin.value()
            self.timer.start(interval)
            self.log(f'开始定时读取，间隔 {interval}ms')
        else:
            self.timer.stop()
            self.log('停止定时读取')

    def auto_read_registers(self):
        """定时自动读取寄存器"""
        if not self.client or not self.client.is_socket_open():
            self.timer.stop()
            self.auto_read_check.setChecked(False)
            self.log('连接已断开，停止定时读取')
            return

        slave_id = self.slave_id_spin.value()
        address = self.start_addr_edit.value()
        count = self.read_count_spin.value()

        try:
            result = self.client.read_holding_registers(
                address=address,
                count=count,
                slave=slave_id
            )

            if not result.isError():
                registers = result.registers
                hex_values = [f"0x{x:04X}" for x in registers]
                self.log(f'自动读取 地址 0x{address:04X}-0x{address + count - 1:04X}: {hex_values}')
            else:
                self.log(f'自动读取错误: {result}')
        except Exception as e:
            self.log(f'自动读取异常: {str(e)}')
            self.timer.stop()
            self.auto_read_check.setChecked(False)

    def execute_command(self):
        if not self.client or not self.client.is_socket_open():
            self.log('错误: 未连接到设备')
            return

        func_index = self.func_code_cb.currentIndex()
        slave_id = self.slave_id_spin.value()
        address = self.addr_edit.value()  # 获取十六进制转换后的十进制值
        count = self.count_spin.value()
        values = self.value_edit.text()

        try:
            if func_index == 0:  # 读取线圈
                result = self.client.read_coils(address=address, count=count, slave=slave_id)
                if not result.isError():
                    self.log(f'地址 0x{address:04X} 线圈状态: {result.bits}')
            elif func_index == 1:  # 读取输入状态
                result = self.client.read_discrete_inputs(address=address, count=count, slave=slave_id)
                if not result.isError():
                    self.log(f'地址 0x{address:04X} 输入状态: {result.bits}')
            elif func_index == 2:  # 读取保持寄存器
                result = self.client.read_holding_registers(address=address, count=count, slave=slave_id)
                if not result.isError():
                    self.log(f'地址 0x{address:04X} 保持寄存器: {[f"0x{x:04X}" for x in result.registers]}')
            elif func_index == 3:  # 读取输入寄存器
                result = self.client.read_input_registers(address=address, count=count, slave=slave_id)
                if not result.isError():
                    self.log(f'地址 0x{address:04X} 输入寄存器: {[f"0x{x:04X}" for x in result.registers]}')
            elif func_index == 4:  # 写入单个线圈
                value = True if values.lower() in ['1', 'true', 'on'] else False
                result = self.client.write_coil(address=address, value=value, slave=slave_id)
                if not result.isError():
                    self.log(f'地址 0x{address:04X} 写入单个线圈成功')
            elif func_index == 5:  # 写入单个寄存器
                value = int(values)
                result = self.client.write_register(address=address, value=value, slave=slave_id)
                if not result.isError():
                    self.log(f'地址 0x{address:04X} 写入单个寄存器成功')
            elif func_index == 6:  # 写入多个线圈
                values = [v.strip().lower() in ['1', 'true', 'on'] for v in values.split(',')]
                result = self.client.write_coils(address=address, values=values, slave=slave_id)
                if not result.isError():
                    self.log(f'地址 0x{address:04X} 写入多个线圈成功')
            elif func_index == 7:  # 写入多个寄存器
                values = [int(v.strip()) for v in values.split(',')]
                result = self.client.write_registers(address=address, values=values, slave=slave_id)
                if not result.isError():
                    self.log(f'地址 0x{address:04X} 写入多个寄存器成功')

            if result.isError():
                self.log(f'Modbus错误: {result}')

        except Exception as e:
            self.log(f'执行错误: {str(e)}')

    def log(self, message):
        self.log_edit.append(message)

    def closeEvent(self, event):
        if self.client and self.client.is_socket_open():
            self.client.close()
        if self.timer.isActive():
            self.timer.stop()
        event.accept()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = ModbusMasterGUI()
    ex.show()
    sys.exit(app.exec_())