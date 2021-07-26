/*-----------------GUI para laboratorio 2, Electrónica Digital 2 ------------------
----------------------------- Andy Bonilla, 19451 ---------------------------------
-------------Interfaz grafica para control y monitoreo de sensores---------------*/

/*----------------------------------------------------------------------------------
-------------------------------librerias a implementar------------------------------
----------------------------------------------------------------------------------*/
import processing.serial.*;    //se importa libreria de comunicacion serial

/*----------------------------------------------------------------------------------
---------------------------definicion de variables a implementar--------------------
----------------------------------------------------------------------------------*/
Serial myPort;                        //variable objeto para comunicacion serial

/*----------------------------------------------------------------------------------
-----------------------------definicion de cadenas a implementar--------------------
----------------------------------------------------------------------------------*/
int boton1[] = {150,150,60};             //punto en x, punto en y,tamaño0
int led1[] = {150,100,50};                         //puntox, puntoy,tamaño
int boton_eeprom[] = {220,425, 60};              //puntox, puntoy y tamaño

/*----------------------------------------------------------------------------------
---------------------------definicion de variables a implementar--------------------
----------------------------------------------------------------------------------*/
int dato_transmitido=0;    //variable para mandar al PIC

/*----------------------------------------------------------------------------------
---------------------------------implementacion de funciones------------------------
----------------------------------------------------------------------------------*/

//--------------------------------configuracion general-----------------------------
void setup()
{
//------------------Configuracion de puertos seriales
//puerto = Serial.list()[0];                 //declaraccion de puerto USB para USARTcom
/*myPort = new Serial(this, Serial.list()[0], 9600);          //configuracion de puerto y braudeaje
myPort.buffer(6);
thread("serial_comm");*/

  
size(550,580);                                      //tamaño de 500*500 pixeles
background(100,100,100);                            //color de fondo
textSize(20);                                       //tamaño de texto en pantalla
text("Laboratorio 2, Electronica digital 2", 120, 50);
text("LCD y UART",200,80);
textSize(15);      
text("No es mucho pero es trabajo honesto", 120, 300);
}

//--------------------------funcion de dibujo ---------------------------------------
void draw()
{
  fill(1300,1300,1300);      //se rellena el fondo del dibujo
  
  //------------------------DIBUJO DE BOTON PARA SUMAR
  fill(300,300,300);    //se rellena el color
  textSize(15);                                       //tamaño de texto en pantalla
  text("Suma",boton1[0]+12, boton1[1]-10);
  square(boton1[0],boton1[1],boton1[2]);
  if (mousePressed)
  {
    if (mouseX > boton1[0] && mouseX < boton1[0]+boton1[2])
    {
      if (mouseY > boton1[1] && mouseY < boton1[1]+boton1[2])
      {
        fill(255,0,0);
        square(boton1[0],boton1[1],boton1[2]);
        dato_transmitido++;
        if (dato_transmitido ==255)
        {
          dato_transmitido=255;
          println(dato_transmitido);
        }
        //myPort.write(dato_transmitido);       //valor
      }
    }
  }
  
  //------------------------DIBUJO DE BOTON PARA RESTAR
  fill(300,300,300);                                  //
  textSize(15);                                       //tamaño de texto en pantalla
  text("Resta",boton1[0]+132, boton1[1]-10);
  square(boton1[0]+120,boton1[1],boton1[2]);
  if (mousePressed)
  {
    if (mouseX > boton1[0]+120 && mouseX < boton1[0]+boton1[2]+120)
    {
      if (mouseY > boton1[1] && mouseY < boton1[1]+boton1[2])
      {
        fill(255,0,0);
        square(boton1[0]+120,boton1[1],boton1[2]);
        dato_transmitido--;
        if (dato_transmitido < 0)
        {
          dato_transmitido=0;
          println(dato_transmitido);
        }
        println("0");
        //println(dato_transmitido);
        //myPort.write(dato_transmitido);
      }
    }
  }
  println(dato_transmitido);
  if (dato_transmitido ==0)
    dato_transmitido=0;
  if (dato_transmitido >=255)
    dato_transmitido=255;
}



//--------------------------funcion de comunicacion serial----------------------------

/*void serialEvent(Serial myPort)
{
  //myPort.readBytes(dePic);
  for (int i=0;i<6 ;i++)
  {
    dePicInt[i] = dePic[i] & 0xFF;
  }
  
}

void serial_comm()
{
  while (true)
  {
    currentMillis = millis();
  
    if (currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;
      for (int i = 0; i<6;i++)
      {
        haciaPic[i]=byte(haciaPicInt[i]);
      }
      myPort.write(haciaPic);
    }
  }
}*/
