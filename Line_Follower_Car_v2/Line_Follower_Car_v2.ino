// Xe dò line 5 cảm biến
// Make by Nguyen Hoang Viet ( VietLinh-OXO)


/*                          SƠ ĐỒ CẢM BIẾN TRÊN XE

    |cambien1|      |cambien2|      |cambien3|      |cambien4|      |cambien5|

*/
//Khai báo các giá trị PID
float Kp = 1;
float Ki = 0;
float Kd = 1;
float error = 0, P = 0, I = 0, D = 0, PID_value = 0;
float previous_error = 0, previous_I = 0;
// khai báo các biến của hàm millis , dùng millis thay thế delay
unsigned long hien_tai = 0;
unsigned long thoi_gian;
int thoi_gian_cho = 30;  // thời gian để đọc giá trị cảm biến
char gia_tri_xe;

int mau_vach = 1;  // nếu vạch là màu đen thì để giá trị là 1 , vạch màu trắng là 0
// khai báo các chân kết nối  của cảm biến
const int cambien1 = ;  // kết nối với chân out của cảm biến số 1
const int cambien2 = ;  // kết nối với chân out của cảm biến số 2
const int cambien3 = ;  // kết nối với chân out của cảm biến số 3
const int cambien4 = ;  // kết nối với chân out của cảm biến số 4
const int cambien5 = ;  // kết nối với chân out của cảm biến số 5
int gtcb[5] = { 0, 0, 0, 0, 0 };
//khai báo các chân kết nối của l298N
const int enA = ;   // kết nối với chân ENA của L298N
const int in1 = ;   // kết nối với chân IN_1(hay IN_A) của L298N
const int in2 = ;   // kết nối với chân IN_2(hay IN_B) của L298N
const int in3 = ;   // kết nối với chân IN_3(hay IN_C) của L298N
const int in4 = ;   // kết nối với chân IN_4(hay IN_D) của L298N
const int enB = ;  // kết nối với chân ENB của L298N
// đặt tốc độ mặc định của động cơ
int tocdo = 20;  // tốc độ mặc định của xe 1-255
int flag = 0;
void doc_gia_tri_cam_bien();
void calculate_PID();
void motor_ctrl();

void setup() {
  // Thiết lập nhiệm vụ của các chân , hoặc các đoạn code chỉ chạy một lần
  pinMode(cambien1, INPUT);
  pinMode(cambien2, INPUT);
  pinMode(cambien3, INPUT);
  pinMode(cambien4, INPUT);
  pinMode(cambien5, INPUT);

  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  // đặt giá trị ban đầu của động cơ là 0 , xe dừng tránh hoạt động bất ngờ
  analogWrite(enA, 0);
  analogWrite(enB, 0);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  // dừng hệ thống khoảng 1,5 giây để chuẩn bị
  Serial.begin(9600);
  Serial.println(" READY ");

  delay(1000);
  Serial.println(" STARTED !!!! ");
  delay(500);
}

void loop() {

  doc_gia_tri_cam_bien();
 /* if(Serial.available())
   { 
    Serial.print(gtcb[1]);                 
    Serial.print("  ,  ");
    Serial.print(gtcb[2]);
    Serial.print("  ,  ");                   // Xuất giá trị của các cảm biến 
    Serial.print(gtcb[3]);
    Serial.print("  ,  ");
    Serial.print(gtcb[4]);
    Serial.print("  ,  ");
    Serial.println(gtcb[5]);
    }
    */
  if (error == 50) {
    do {
      disangtrai();
      analogWrite(enA, 150);
      analogWrite(enB, 0);
      doc_gia_tri_cam_bien();
    } while (error == 0);
  } else if (error == -50) {
    do {
      disangphai();
      analogWrite(enA, 0);
      analogWrite(enB, 150);
      doc_gia_tri_cam_bien();
    } while (error == 0);
  } else if (error == 100) {
    do {
      doc_gia_tri_cam_bien();
      analogWrite(enA, 255);
      analogWrite(enB, 255);
      xoaysangphai();
    } while (error == 0);
  } else if (error == 200) {
    dunglai();
    if (flag == 0) {
      analogWrite(enA, 150);
      analogWrite(enB, 80);
      dithang();
      delay(100);
      doc_gia_tri_cam_bien();
      if (error == 200) {
        dunglai();
        delay(100);
        flag = 1;
      } else {
        do {
          analogWrite(enA, 150);
          analogWrite(enB, 80);
          xoaysangtrai();
          doc_gia_tri_cam_bien();
        } while (error == 0);
      }
    }
  } else {
    calculate_PID();
    motor_ctrl();
  }
}

void doc_gia_tri_cam_bien() {
  if (thoi_gian - hien_tai > thoi_gian_cho) {
    hien_tai = millis();
    gtcb[0] = digitalRead(cambien1);  //Đọc giá trị cảm biến số 1
    gtcb[1] = digitalRead(cambien2);  //Đọc giá trị cảm biến số 2
    gtcb[2] = digitalRead(cambien3);  //Đọc giá trị cảm biến số 3
    gtcb[3] = digitalRead(cambien4);  //Đọc giá trị cảm biến số 4
    gtcb[4] = digitalRead(cambien5);  //Đọc giá trị cảm biến số 5
  }
  if ((gtcb[0] != mau_vach) && (gtcb[1] == mau_vach) && (gtcb[2] == mau_vach) && (gtcb[3] == mau_vach) && (gtcb[4] == mau_vach))  // 0 1 1 1 1
    error = 60;
  else if ((gtcb[0] != mau_vach) && (gtcb[1] != mau_vach) && (gtcb[2] == mau_vach) && (gtcb[3] == mau_vach) && (gtcb[4] == mau_vach))  // 0 0 1 1 1
    error = 50;
  else if ((gtcb[0] != mau_vach) && (gtcb[1] != mau_vach) && (gtcb[2] != mau_vach) && (gtcb[3] != mau_vach) && (gtcb[4] == mau_vach))  // 0 0 0 0 1
    error = 40;
  else if ((gtcb[0] != mau_vach) && (gtcb[1] != mau_vach) && (gtcb[2] != mau_vach) && (gtcb[3] == mau_vach) && (gtcb[4] == mau_vach))  // 0 0 0 1 1
    error = 30;
  else if ((gtcb[0] != mau_vach) && (gtcb[1] != mau_vach) && (gtcb[2] != mau_vach) && (gtcb[3] == mau_vach) && (gtcb[4] != mau_vach))  // 0 0 0 1 0
    error = 20;
  else if ((gtcb[0] != mau_vach) && (gtcb[1] != mau_vach) && (gtcb[2] == mau_vach) && (gtcb[3] == mau_vach) && (gtcb[4] != mau_vach))  // 0 0 1 1 0
    error = 10;
  else if ((gtcb[0] != mau_vach) && (gtcb[1] != mau_vach) && (gtcb[2] == mau_vach) && (gtcb[3] != mau_vach) && (gtcb[4] != mau_vach))  // 0 0 1 0 0
    error = 0;
  else if ((gtcb[0] != mau_vach) && (gtcb[1] == mau_vach) && (gtcb[2] == mau_vach) && (gtcb[3] != mau_vach) && (gtcb[4] != mau_vach))  // 0 1 1 0 0
    error = -10;
  else if ((gtcb[0] != mau_vach) && (gtcb[1] == mau_vach) && (gtcb[2] != mau_vach) && (gtcb[3] != mau_vach) && (gtcb[4] != mau_vach))  // 0 1 0 0 0
    error = -20;
  else if ((gtcb[0] == mau_vach) && (gtcb[1] == mau_vach) && (gtcb[2] != mau_vach) && (gtcb[3] != mau_vach) && (gtcb[4] != mau_vach))  // 1 1 0 0 0
    error = -3;
  else if ((gtcb[0] == mau_vach) && (gtcb[1] != mau_vach) && (gtcb[2] != mau_vach) && (gtcb[3] != mau_vach) && (gtcb[4] != mau_vach))  // 1 0 0 0 0
    error = -40;
  else if ((gtcb[0] == mau_vach) && (gtcb[1] == mau_vach) && (gtcb[2] == mau_vach) && (gtcb[3] != mau_vach) && (gtcb[4] != mau_vach))  // 1 1 1 0 0
    error = -50;
  else if ((gtcb[0] == mau_vach) && (gtcb[1] == mau_vach) && (gtcb[2] == mau_vach) && (gtcb[3] == mau_vach) && (gtcb[4] != mau_vach))  // 1 1 1 1 0
    error = -60;
  else if ((gtcb[0] != mau_vach) && (gtcb[1] != mau_vach) && (gtcb[2] != mau_vach) && (gtcb[3] != mau_vach) && (gtcb[4] != mau_vach))  // 0 0 0 0 0
    error = 100;
  else if ((gtcb[0] == mau_vach) && (gtcb[1] == mau_vach) && (gtcb[2] == mau_vach) && (gtcb[3] == mau_vach) && (gtcb[4] == mau_vach))  // 1 1 1 1 1
    error = 200;
}

void calculate_PID() {
  P = error;
  I = I + previous_I;
  D = error - previous_error;

  PID_value = (Kp * P) + (Ki * I) + (Kd * D);

  previous_I = I;
  previous_error = error;
}
void motor_ctrl() {
  int left_motor_speed = tocdo + PID_value;
  int right_motor_speed = tocdo - PID_value;

  left_motor_speed = constrain(left_motor_speed, 0, 255);    // đặt giá trị cao nhất và thấp nhất cho chân enB
  right_motor_speed = constrain(right_motor_speed, 0, 255);  // đặt giá trị cao nhất và thấp nhất cho chân enA

  analogWrite(enA, right_motor_speed);
  analogWrite(enB, left_motor_speed);
  dithang();
}
void dithang() {
  digitalWrite(in1, 0);
  digitalWrite(in2, 1);
  digitalWrite(in3, 0);
  digitalWrite(in4, 1);
}

void xoaysangtrai() {
  digitalWrite(in1, 1);
  digitalWrite(in2, 0);
  digitalWrite(in3, 0);
  digitalWrite(in4, 1);
}

void xoaysangphai() {
  digitalWrite(in1, 0);
  digitalWrite(in2, 1);
  digitalWrite(in3, 1);
  digitalWrite(in4, 0);
}

void disangtrai() {
  digitalWrite(in1, 1);
  digitalWrite(in2, 0);
  digitalWrite(in3, 0);
  digitalWrite(in4, 0);
}

void disangphai() {
  digitalWrite(in1, 0);
  digitalWrite(in2, 0);
  digitalWrite(in3, 1);
  digitalWrite(in4, 0);
}

void dunglai() {
  digitalWrite(in1, 0);
  digitalWrite(in2, 0);
  digitalWrite(in3, 0);
  digitalWrite(in4, 0);
}