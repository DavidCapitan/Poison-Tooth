#include <Keyboard.h>

void setup() {
  //pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, HIGH);
  Keyboard.begin();//comienzo de simulación de teclado
  delay(500);//delay necesario para que funcionen las siguientes intrucciones (si no no se ejecuta el windows + r)
  Keyboard.press(KEY_LEFT_GUI);// Presionar tecla de Windows
  Keyboard.press('r');// Presionar tecla 'r' para abrir la pestaña de ejecución
  Keyboard.releaseAll();// soltar todas las teclas
  delay(100);//delay necesario para que al sistema le de tiempo a abrir ejecutar

  Keyboard.println("notepad"); // Escribir "notepad" y gracias al retorno de carga del ln hacemos enter
  delay(300);//tiempo de espera a que se abra el blog de notas

  // Escribir "hola" y presionar Enter
  Keyboard.print("hola");
  Keyboard.end();//fin del programa
  //digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // No hay código en el loop en este ejemplo
}


/*#include <Keyboard.h>

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Keyboard.begin();
  delay(3000);

  // Presionar tecla de Windows y 'r' para abrir la ejecución
  Keyboard.press(KEY_LEFT_GUI);
  delay(1000);
  Keyboard.press('r');
  delay(1000);
  Keyboard.releaseAll();
  delay(1000);

  // Escribir "notepad" y presionar Enter
  Keyboard.print("notepad");
  delay(1000);
  Enter();
  delay(2000);

  // Escribir "hola" y presionar Enter
  Keyboard.println("hola");
  delay(2000);
  Enter();

  delay(500);
  Keyboard.end();
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // No hay código en el loop en este ejemplo
}

void Enter() {
  Keyboard.press(KEY_RETURN);
  delay(1000);
  Keyboard.release(KEY_RETURN);
  delay(1000);
}*/
