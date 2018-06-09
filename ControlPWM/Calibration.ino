void calibration(){  
  Serial.println("Manette gauche verticale à fond vers l'avant");
  delay(3000);
  canal2_min=receiver_input[2];
  Serial.println(canal2_min);
  Serial.println("Manette gauche verticale à fond vers l'arrière");
  delay(3000);
  canal2_max=receiver_input[2];
  Serial.println(canal2_max);
  moyenne[2]=(canal2_min+canal2_max)/2;
  
  Serial.println("Manette gauche horizontale à fond vers la gauche");
  delay(3000);
  canal4_min=receiver_input[4];
  Serial.println(canal4_min);
  Serial.println("Manette gauche horizontale à fond vers la droite");
  delay(3000);
  canal4_max=receiver_input[4];
  Serial.println(canal4_max);
  moyenne[4]=(canal4_min+canal4_max)/2;
  
  Serial.println("Manette droite verticale à fond vers l'avant");
  delay(3000);
  canal3_min=receiver_input[3];
  Serial.println(canal3_min);
  Serial.println("Manette droite verticale à fond vers l'arrière");
  delay(3000);
  canal3_max=receiver_input[3];
  Serial.println(canal3_max);
  moyenne[3]=(canal3_min+canal3_max)/2;
  
  Serial.println("Manette droite horizontale à fond vers la gauche");
  delay(3000);
  canal1_max=receiver_input[1];
  Serial.println(canal1_max);
  Serial.println("Manette droite horizontale à fond vers la droite");
  delay(3000);
  canal1_min=receiver_input[1];
  Serial.println(canal1_min);
  moyenne[1]=(canal1_min+canal1_max)/2;
}


