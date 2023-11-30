#include <Keypad.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>

#define CUSTOM_SDA_PIN 5
#define CUSTOM_SCL_PIN 6
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const byte ROWS = 3;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'U', 'L', 'D'},
  {'R', 'S', 'T'},
  {'A', 'B', ' '}
};

int nb_joueurs = 1;
bool lobbyEnCours = true; // Variable pour gérer l'état du lobby

int plateauJeu[8][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
};

const char *ssid = "PoleDeVinci_DVIC";
const char *password = "8PfURsp!dvic";
byte rowPins[ROWS] = {20, 10, 0};
byte colPins[COLS] = {7, 8, 9};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

WiFiServer server(80);
WiFiClient client;

int selectorX = 0;
int selectorY = 0;

bool grid[8][8] = {false};

unsigned long startTime = 0;
const unsigned long displayTime = 2000; // 2 secondes

void setup() {
  Serial.begin(9600);
  Wire.begin(CUSTOM_SDA_PIN, CUSTOM_SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Allocation SSD1306 échouée"));
    for (;;)
      ;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 20);
  display.print("Othello");
  display.display();

  startTime = millis(); // Enregistrer le temps de début
  delay(2000); // Attendre 2 secondes
  display.clearDisplay(); // Effacer l'écran
  WiFi.softAP(ssid, password);
  IPAddress ip = WiFi.softAPIP();
  Serial.print("Adresse IP du serveur: ");
  Serial.println(ip);
  server.begin();
}

void loop() {
  if (lobbyEnCours) {
    // Afficher un message d'attente dans le lobby
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 20);
    display.print("En attente de joueurs...");
    display.display();

    // Vérifier si un nouveau client s'est connecté
    if (server.hasClient()) {
      client = server.available();
      if (client && client.connected()) {
        Serial.println("Nouveau client connecté");
        nb_joueurs++;
        Serial.println(nb_joueurs);
        if (nb_joueurs == 2) {
          lobbyEnCours = false;
          display.clearDisplay();
          display.setTextSize(1);
          display.setTextColor(SSD1306_WHITE);
          display.setCursor(20, 20);
          display.print("Joueur connecté !");
          display.display();
        }
        Serial.print("lobby: ");
        Serial.print(lobbyEnCours);
      }
    }
  } else {
    // Logique du jeu ici
    char key = keypad.getKey();
    if (key){
    
      switch (key) {
      case 'U':
        selectorY = max(0, selectorY - 1);
        updateDisplay();
        break;
      case 'D':
        selectorY = min(7, selectorY + 1);
        updateDisplay();
        break;
      case 'L':
        selectorX = max(0, selectorX - 1);
        updateDisplay();
        break;
      case 'R':
        selectorX = min(7, selectorX + 1);
        updateDisplay();
        break;
      case 'A':
        grid[selectorX][selectorY] = true;
        plateauJeu[selectorX][selectorY] = 1;
        updateDisplay();
        String positionMessage = String(selectorX)+String(selectorY);
        client.println(positionMessage);
        if (client && client.connected()) {
          while (!client.available()) {
          String command = client.readString();
          char x_client = command[0];
          char y_client = command[1];
          updateClient(x_client,y_client);
          break;
      }
      }
    }
  }
  }
}

void updateDisplay() {
  display.clearDisplay();
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      display.drawRect(x * (SCREEN_WIDTH / 8), y * (SCREEN_HEIGHT / 8), SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8, SSD1306_WHITE);
      if (grid[x][y]) {
        display.fillCircle(x * (SCREEN_WIDTH / 8) + (SCREEN_WIDTH / 16), y * (SCREEN_HEIGHT / 8) + (SCREEN_HEIGHT / 16), 2, SSD1306_WHITE);
      }
    }
  }
  display.drawRect(selectorX * (SCREEN_WIDTH / 8), selectorY * (SCREEN_HEIGHT / 8), SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8, SSD1306_INVERSE);
  display.display();
}

void updateClient(int clientX, int clientY)
{
   Serial.print("Message envoyé par le client :");
   Serial.print(clientX);
   Serial.println(clientY);

   display.clearDisplay();
   for (int x = 0; x < 8; x++) {
     for (int y = 0; y < 8; y++) {
       display.drawRect(x * (SCREEN_WIDTH / 8), y * (SCREEN_HEIGHT / 8), SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8, SSD1306_WHITE);
       if (grid[x][y]) {
         // Dessiner un triangle au lieu d'un cercle
         int centerX = x * (SCREEN_WIDTH / 8) + (SCREEN_WIDTH / 16);
         int centerY = y * (SCREEN_HEIGHT / 8) + (SCREEN_HEIGHT / 16);
         int triangleSize = 4; // Taille du triangle

         // Coordonnées des sommets du triangle
         int x1Triangle = centerX - triangleSize;
         int y1Triangle = centerY + triangleSize;
         int x2Triangle = centerX + triangleSize;
         int y2Triangle = centerY + triangleSize;
         int x3Triangle = centerX;
         int y3Triangle = centerY - triangleSize;

         display.drawTriangle(x1Triangle, y1Triangle, x2Triangle, y2Triangle, x3Triangle, y3Triangle, SSD1306_WHITE);
       }
     }
   }
   // Dessiner le sélecteur
   display.drawRect(clientX * (SCREEN_WIDTH / 8), clientY * (SCREEN_HEIGHT / 8), SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8, SSD1306_INVERSE);
   display.display();
}
