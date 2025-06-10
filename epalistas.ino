#include "mp3tf16p.h"

// Pin για αισθητήρα ΜΕΣΑ στην τουαλέτα
#define TRIG_TOILET 8
#define ECHO_TOILET 9

// Pin για αισθητήρα στον ΠΡΟΘΑΛΑΜΟ
#define TRIG_ENTRANCE 6
#define ECHO_ENTRANCE 7

// LED ένδειξης κατάστασης τοποθετημένα στον ΠΡΟΘΑΛΑΜΟ
#define GREEN_LED 3
#define RED_LED 4

// Αντικείμενο MP3Player για τον DFPlayer Mini
MP3Player mp3(10, 11);  // TX, RX (μέσω αντιστάσεων)

bool personDetected = false;

void setup() {
  // Ρύθμιση pin για αισθητήρες
  pinMode(TRIG_TOILET, OUTPUT);
  pinMode(ECHO_TOILET, INPUT);
  pinMode(TRIG_ENTRANCE, OUTPUT);
  pinMode(ECHO_ENTRANCE, INPUT);

  // Ρύθμιση LED εξόδου
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  Serial.begin(9600);       // Σειριακή επικοινωνία για παρακολούθηση
  mp3.initialize();         // Εκκίνηση DFPlayer
}

// Συνάρτηση για μέτρηση απόστασης με αισθητήρα υπερήχων
long measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;  // μετατροπή σε εκατοστά
}

void loop() {
  // Μέτρηση απόστασης στον ΠΡΟΘΑΛΑΜΟ
  long entranceDistance = measureDistance(TRIG_ENTRANCE, ECHO_ENTRANCE);

  // Αν κάποιος βρίσκεται στον προθάλαμο (π.χ. απόσταση < 60cm)
  if (entranceDistance > 0 && entranceDistance < 60) {
    if (!personDetected) {
      personDetected = true;

      // Ελέγχει την κατάσταση ΜΕΣΑ στην τουαλέτα
      long toiletDistance = measureDistance(TRIG_TOILET, ECHO_TOILET);

      if (toiletDistance > 0 && toiletDistance < 10) {
        // Τουαλέτα κατειλημμένη
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(RED_LED, HIGH);
        Serial.println("Η τουαλέτα είναι κατειλημμένη");
        mp3.playTrackNumber(1, 15);  // Track 1: "Η τουαλέτα είναι κατειλημμένη"
      } else {
        // Τουαλέτα ελεύθερη
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
        Serial.println("Η τουαλέτα είναι διαθέσιμη");
        mp3.playTrackNumber(2, 15);  // Track 2: "Η τουαλέτα είναι διαθέσιμη"
      }

      delay(3000);  // Προστασία από διπλές ενεργοποιήσεις
    }
  } else {
    // Αν δεν υπάρχει κανείς στον προθάλαμο
    personDetected = false;
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
  }

  delay(200);  // Μικρή παύση για σταθερή λειτουργία
}
