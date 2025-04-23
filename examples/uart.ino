'''
串口波特率115200 
串口输入：[speed]\n       -255<speed<255
电机即可旋转
'''

// TB6612电机驱动配置
#define MOTOR_IN1 10   // PIN10->AIN1
#define MOTOR_IN2 11   // PIN11->AIN2
#define MOTOR_PWM 9    // PIN9 ->PWMA

// 串口通信参数(这里使用了UART1 PIN6 PIN7)
const int BAUD_RATE = 115200;
unsigned long lastUpdateTime = 0;
const unsigned long POLL_INTERVAL = 20; // 非阻塞轮询间隔(ms)

String inputBuffer = "";        
bool newDataAvailable = false; 

void setup() {
  // 初始化电机控制引脚
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_PWM, OUTPUT);

  // 初始化串口通信
  Serial.begin(BAUD_RATE);
  Serial.setTimeout(10); // 设置超时防止阻塞
  
  // 电机初始状态：停止
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  analogWrite(MOTOR_PWM, 0);
}

void loop() {
  // 非阻塞式串口读取
  receiveSerialData();
  
  // 定时处理逻辑（20ms周期）
  if (millis() - lastUpdateTime >= POLL_INTERVAL) {
    handleMotorControl();
    lastUpdateTime = millis();
  }
}

void receiveSerialData() {
  while (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      newDataAvailable = true;
      break;
    }
    inputBuffer += inChar;
  }
}

void handleMotorControl() {
  if (newDataAvailable) {
    // 解析转速指令（-255~255）
    int targetSpeed = inputBuffer.toInt();
    targetSpeed = constrain(targetSpeed, -255, 255);
    
    // 设置电机方向
    if (targetSpeed > 0) {
      digitalWrite(MOTOR_IN1, HIGH);
      digitalWrite(MOTOR_IN2, LOW);
    } else if (targetSpeed < 0) {
      digitalWrite(MOTOR_IN1, LOW);
      digitalWrite(MOTOR_IN2, HIGH);
    } else {
      digitalWrite(MOTOR_IN1, LOW);
      digitalWrite(MOTOR_IN2, LOW);
    }
    
    // 设置PWM占空比（取绝对值）
    analogWrite(MOTOR_PWM, abs(targetSpeed));
    
    // 清空缓存
    inputBuffer = "";
    newDataAvailable = false;
    
    // 反馈当前转速
    Serial.print("Speed set to: ");
    Serial.println(targetSpeed);
  }
}