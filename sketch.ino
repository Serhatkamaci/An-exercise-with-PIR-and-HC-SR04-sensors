#include <WiFi.h> // WiFi ile ağa bağlanmak için kullanıldı.
#include <PubSubClient.h> // MQTT brokera bağlanmak için kullanıldı.

long duration; // Süre değişkeni için kullanıldı.
float distance; // Mesafe değişkeni için kullanıldı.
int trigPin = 25; // HC-SRO4 sensörünün trig pini için kullanıldı.
int echoPin = 34; // HC-SRO4 sensörünün echo pini için kullanıldı.

int pir = 4; // Pir sensörünün pir! pini için kullanıldı.
int redPin = 5;  // RGB Led'in red pini için kullanıldı.
int greenPin = 17; // RGB Led'in green pini için kullanıldı.   
int bluePin = 16; // RGB Led'in blue pini için kullanıldı.

char* ssid="Wokwi-GUEST"; // Ağ adı için kullanıldı.
char* password=""; // Ağ şifresi için kullanıldı.

char* mqttServer="test.mosquitto.org"; // MQTT server'ına bağlanmak için kullanıldı.
char* topic="20010011057/mesafe"; // MQTT mesajlarını göndermek ve almak için kullanıldı.

int IP=0; // Hareket değişkeni için kullanıldı.

WiFiClient espClient; // WiFi üzerinden TCP bağlantıları kurmak için kullanıldı.
PubSubClient client(espClient); // MQTT borker'ına bağlanmak için kullanıldı.

String msgStr; // MQTT broker'a gönderilen mesajlar için kullanıldı.
boolean led_switch=false; // Dashboard'daki switch'i tutmak için kullanıldı. 

void setup_wifi()
{
    delay((10)); // 10 milisaniye bekletildi.
    WiFi.begin(ssid,password); // WiFi ağına bağlanmaya çalışıldı.

    while(WiFi.status()!=WL_CONNECTED) // WiFi bağlantısı kurulana kadar devam eder.
    {
        delay(500); // 500 milisaniye bekletildi.
    }

    Serial.println("WiFi'ye bağlanıldı!"); // Terminale "WiFi'ye bağlanıldı!" yazısı yazıldı.
    Serial.print("IP adress: "); // Terminale "IP adress: " yazısı yazıldı.
    Serial.println(WiFi.localIP()); // Terminale yerel ağın IP adresi yazıldı.
}

void reconnected() // MQTT broker'a tekrardan bağlanmak için kullanıldı.
{
    while(!client.connected()) // İstemcinin bağlantı durumunu kontrol eder.
    {
        String istemciId = "ESP32Client-"; // İstemci kimlik değerini tutması için kullanıldı.
        istemciId += String(random(0xffff), HEX); // Her bağlantıda benzersiz bir kimlik oluşturulması için kullanıldı.   
        if(client.connect(istemciId.c_str())) // MQTT borker'a bağlanmaya çalışır. Eğer sonuç true olursa bu bağlama girer.
        {
            Serial.println("MQTT'ye bağlanıldı!"); // Terminale "MQTT'ye bağlanıldı!" yazısı yazıldı.
            Serial.println(); // Terminalde bir satır aşağı geçildi.
            client.subscribe("20010011057/LED"); // İstemci "20010011057/LED" konusu ile abone oldu.
            client.subscribe(topic); // İstemci "20010011057/mesafe" konusu ile abone oldu.
            client.publish(topic, "0.0");  // İstemci "20010011057/mesafe" konusuna 0.0 mesajı yayınladı.
        }
        else
        {
            Serial.println("Hata"); // Bağlantı sağlanmaz ise terminale "Hata" mesajı yazıldı.
            Serial.println("5 saniye içersinde tekrardan bağlanılmaya çalışılacak!"); // Terminale "5 saniye içersinde tekrardan bağlanılmaya çalışılacak!" yazısı yazıldı.
            delay(5000); // 5000 milisaniye bekletildi.
        }
    }
}

void renkAyarla(int kirmizi, int yesil, int mavi)
{ 
  // Ledin parlaklığının doğru bir şekilde ayarlanması için renk değerlerinin terse çevirilme işlemlerinin yapılması sağlanıldı.
  kirmizi = 255-kirmizi; // Kırmızı renk değeri çevirildi.
  yesil = 255-yesil; // Yeşil renk değeri çevirildi.
  mavi = 255-mavi; // Mavi renk değeri çevirildi.
  analogWrite(redPin, kirmizi); // Terse çevirilmiş kırmızı renk değeri redPin'ine yazıldı.
  analogWrite(greenPin, yesil); // Terse çevirilmiş yeşil renk değeri greenPin'ine yazıldı.
  analogWrite(bluePin, mavi); // Terse çevirilmiş mavi renk değeri bluePin'ine yazıldı.
  
}

void callback(char* topic, byte* payload, unsigned int length)
{
    // MQTT mesajlarını dinler konuya bir mesaj geldiyse ilgili işlemleri gerçekleştirir.
    Serial.print("Mesaj geldi: ");// Terminale "Mesaj geldi: " yazısı yazıldı.
    Serial.print(topic);// Terminale mesajın konusu yazıldı.
    String data=""; // Gelen mesajı tutmak için kullanıldı.

    for(int i=0; i<length; i++)
    {
        data +=(char)payload[i]; // Gelen mesaj içeriğini data değişkenine yazmak için kullanıldı.
    }

    Serial.print("--> "+data); // Terminale  "--> " ve data değeri yazıldı.

    Serial.println(); // Bir satır aşağı geçmek için kullanıldı.
    if(String(topic)=="20010011057/LED") // Mesajın konusu "20010011057/LED" ise ilgili işlemler gerçekleştirilmektedir. 
    {
        if(data=="ON") // Gelen mesaj "ON" ise switch açıktır.
        {
            led_switch=true; // Bu yüzden kontrol değişkeni "true" olarak güncellenmiştir.
            renkAyarla(255,0,0); // Led'in krımızı renk ile yanması için kullanıldı.
        }
        else
        {
            led_switch=false; // Gelen mesaj "OFF" ise switch kapalıdır.
            renkAyarla(255,255,255); // Led'i beyaz renk ile yakmak için kullanıldı.
        }
    }
    delay(500); // 500 milisaniye bekletildi.
}
 
void setup() {

  Serial.begin(115200); // Seri haberleşmeyi başlatmak için kullamnıldı.
  Serial.println("ESP32 geliştirme kartına bağlanıldı!"); // Terminale "ESP32 geliştirme kartına bağlanıldı!" yazısı yazıldı.
  pinMode(trigPin, OUTPUT); // Sensörün ses dalgası göndermesini sağlar.
  pinMode(echoPin, INPUT); // Ses dalgasının nesneye çarpıp geri dönmesi sonucundaki mesafeyi hesaplar.
  pinMode(pir, INPUT); // Hareket sensörünün verilerinin almak için pin "INPUT" olarak ayarlandı. 

  pinMode(redPin, OUTPUT); // Kırmızı led pini çıkış olarak ayarlandı.
  pinMode(greenPin, OUTPUT); // Yeşil led pini çıkış olarak ayarlandı.
  pinMode(bluePin, OUTPUT); // Mavi led pini çıkış olarak ayarlandı.

  setup_wifi(); // WiFi'ye bağlanma fonksiyonunu başlatır.
  client.setServer(mqttServer, 1883); // MQTT sunucusunun port ve adres ayarlamasını gerçekleştirir.
  client.setCallback(callback); // Gelen mesajları işlemek için bir arama işlevi.

  renkAyarla(255,255,255); // Led'i beyaz renk ile yakmak için kullanıldı.
}

void loop() {
  
  if(!client.connected()) // MQTT istemcisinin bağlantı kontrolünü gerçekleştirir.
  {
      reconnected(); // İstemci eğer bağlanmadıysa tekrardan bağlantı gerçekleşmesi için fonskiyonu "reconnected" çağırılır.
  }

  client.loop(); // Bağlantının sürdürülmesi için gerekli bir kod.

  digitalWrite(trigPin, LOW); // Trig pine düşük değer verilir.
  delayMicroseconds(2); // 2 mikrosaniye bekletildi.

  digitalWrite(trigPin, HIGH); // Trig pine yüksek değer verilir.
  delayMicroseconds(10); // 1o mikrosaniye bekletildi.
  digitalWrite(trigPin, LOW); // Trig pine düşük değer verilir.

  duration = pulseIn(echoPin, HIGH); // Yüksek seviyedeki echo sinyallerinin süresi ölçüldü.
  distance = (duration * 0.034) / 2; // Mesafe hesaplandı.
  
  IP = digitalRead(pir); // Hareket algılama sonucu "IP" değikenine yazıldı.

  msgStr=String(distance); // Mesafe değeri stringe çevirildi.
  client.publish(topic, msgStr.c_str()); // MQTT üzerinden "20010011057/mesafe" konusuna mesaj gönderildi.
  msgStr=""; // Mesaj değişkeni güncellendi.

  if(IP == 1) // Eğer hareket sensörü hareket algıldıysa.
  { 
    if(distance>0 && distance<50) // Mesafe 0 ve 50 arasında ise.
    {
      renkAyarla(255,0,0); // Led'i kırmızı renk ile yakmak için kullanıldı.
      delay(250); // 250 milisaniye bekletildi.
      renkAyarla(255,255,255); // Led'i beyaz renk ile yakmak için kullanıldı.
      
      if(led_switch==false) // Dashboard'daki led switch kapalı ise.
      {
        renkAyarla(255,255,255); // Led'i beyaz renk ile yakmak için kullanıldı.
      }
    }
    else // Mesafe 0 ve 50 arasında değilse.
    {
      if(led_switch==false) // Dashboard'daki led switch kapalı ise.
      {
        renkAyarla(255,255,255); // Led'i beyaz renk ile yakmak için kullanıldı.
      }
      else // Dashboard'daki led switch açık ise.
      {
        renkAyarla(255,0,0); // Led'i kırmızı renk ile yakmak için kullanıldı.
      }
    }
  } 
  else  // Eğer hareket sensörü hareket algılamadıysa.
  {
    if(led_switch==false) // Dashboard'daki led switch kapalı ise.
      {
        renkAyarla(255,255,255); // Led'i beyaz renk ile yakmak için kullanıldı.
      }
    else // Dashboard'daki led switch açık ise.
      {
        renkAyarla(255,0,0); // Led'i kırmızı renk ile yakmak için kullanıldı.
      }
  }
}