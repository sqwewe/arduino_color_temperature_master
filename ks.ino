/*---------------- 1 часть ----------------*/
#include "DHT.h" //библиотека для работы с DHT
#include "SPI.h" //библиотека для работы с SPI
#include "Ethernet.h" //библиотека для работы с Ethernet
#include <Adafruit_NeoPixel.h> //библиотека св. ленты


boolean newInfo = 0; //переменная для новой информации
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//вводим mac адрес, обычно он такого вида, может отличаться
IPAddress ip(192, 168, 0, 177);
//вводим любой IP , который не совпадает с вашим
EthernetServer server(80); //инифиализация библиотеки Ethernet server library

#define DHTPIN 3 //Обозначаем номер пина, к которому подключен датчик
DHT dht(DHTPIN, DHT11); //инициируем датчик DHT

#define LED_COUNT 14  //Обозначаем номер пина, к которому подключена лента            
#define LED_PIN 10 //кол-во светодиодов  

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); //изменение первого числа делает расстояние между цветами

/*---------------- 2 часть ----------------*/
void setup() {
pinMode (8, OUTPUT);//инициализируем 8 пин как выход для светодиода
Ethernet.begin (mac, ip);//запускаем сервер с указанными ранее MAC и вашим IP

strip.begin(); //Инициализируем ленту.
 
Serial.begin (9600);
dht. begin();
server. begin();
 
}

/*---------------- 3 часть ----------------*/
void loop() {

float h = dht.readHumidity(); //Считываем влажность в переменную "h"
float t = dht.readTemperature(); //Считываем температуру в переменную "t"

if(t < 19){
    for (int i = 0; i < LED_COUNT; i++)
  {
    strip.setPixelColor(i, strip.Color(0, 150, 255)); // синий
    strip.show();
    delay(50);
  }}
  if(t > 20){
      for (int i = 0; i < LED_COUNT; i++)
  {
    strip.setPixelColor(i, strip.Color(45, 255, 50)); // мята
    strip.show();
    delay(50);
  }}
  if(t > 40){
    for (int i = 0; i < LED_COUNT; i++)
  {
    strip.setPixelColor(i, strip.Color(255, 0, 0)); // орандж .
    strip.show();
    delay(50);
  }}
else{
     for (int b = 50; b < 250; b++) {
      for (int i = 0; i <= LED_COUNT; i++) {
         strip.setPixelColor(i, 250, 250, b * i / 250);
      }
      strip.show();     // отправляем информацию на ленту
      delay(100 );      // задержка для эффекта
   }
  }



EthernetClient client = server.available(); //принимаем данные, посылаемые клиентом
if (client){ //если запрос оканчивается пустой строкой
boolean currentLineIsBlank = true;
//ставим метку об окончании запроса (дословно: текущая линия чиста)
while (client.connected()) { //пока есть соединение с клиентом
if (client.available()) { //если клиент активен
char c = client.read(); //считываем посылаемую информацию в переменную "с"
if (newInfo && c == ' '){
//если переменная новой информации = 1 и "с", в которой записан запрос, равен пустой строке
newInfo = 0; //то обнуляем переменную поступления новой информации
}
if (c == '$'){
// если переменная "с", несущая отправленный нам запрос, содержит символ $
// "$" подразумевает разделение получаемой информации (символов)
newInfo = 1; //то пришла новая информация, ставим метку новой информации в 1
}
//Проверяем содержание URL - присутствует $1 или $2
if (newInfo == 1){ //если есть новая информация
Serial.println (c);
if (c == '1'){ //и "с" содержит 1
Serial.println ("Включить");
digitalWrite (8, HIGH); //то зажигаем светодиод
}

if (c == '2'){ //если "с" содержит 2
Serial.println ("Выключить");
digitalWrite (8, LOW); //гасим светодиод
}
}
if (c == '\n') { //если "с" равен символу новой строки
currentLineIsBlank = true; //то начинаем новую строку
}
else if (c != '\r') {
//иначе, если "с" не равен символу возврата курсора на начало строки
currentLineIsBlank = false; //то получаем символ на текущей строке
}

if (c == '\n' && currentLineIsBlank) { //выводим HTML страницу

/*---------------- 4 часть ----------------*/
client. println ("HTTP/1.1 200 OK"); //заголовочная информация
client. println ("Content-Type: text/html");
client. println ("Connection: close");
client. println ("Refresh: 2"); //автоматическое обновление каждые 2 сек
client. println (); //Это не ошибка, так должно быть
client. println ("<!DOCTYPE HTML>"); //HTML тип документа
client. println ("<html>"); //открытие тега HTML
client. println ("<head> "); //открытие тега Head
client. println ("<meta http-equiv='Content-Type' content='text/html ; charset=utf-8'/> ");
client. print ("<title>HelpDuino Web Server</title>"); //название страницы
client. println ("</head>"); //заголовочная информация
client. println ("<body>");
client. print ("<H1>Дипломный проект</H1>"); //заголовк на странице
client. print ("<a href=\"/$1\"><button>Включить</button></a>");
//кнопка включить
client. print ("<a href=\"/$2\"><button>Выключить</button></a>");
//кнопка выключить
client. println ("<br> <br>"); //перенос на след. строчку
client. println ("<hr/>"); //линия=====================================
client. println ("Tемпература = "); //Температура с DHT 11
client. println (t); //переменная температуры
client. println (" *C");
client. println ("<br> "); //перенос на след. строчку
client. println ("Влажность = "); //Влажность с DHT 11
client. println (h); //переменная влажности
client. println (" %\t");
client. println ("<br> "); //перенос на след. строчку
client. println ("<hr/>"); //линия=====================================
client. println ("</body>");
client. println ("</html>"); //закрываем тег HTML
break; //выход
}
}
}
delay (1); //время на получение новых данных
client. stop(); //закрываем соеднение
}
}
