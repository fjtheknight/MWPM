/*----------DEFINITION DES INVARIABLES-----------*/

#define attente 0 // état 0 -> état d'attente
#define alimentation_en_fil 1 // état 1 -> alimentation en fil métallique
#define descente_verin 2 // état 2 -> déscente du vérin
#define montee_verin 3 // état 3 -> montée du vérin

#define boutonMarcheArret 6     //broche du bouton de démarrage
#define capteurFil 7     //broche du capteur de fin de course : fil métallique
#define capteurLame 8     //broche du capteur de fin de course : lame cisaille
#define capteurVerin 9    //broche du capteur de début de course : vérin
#define moteur A0   //broche de démarrage du moteur
#define verinBas A1     //broche de déscente du vérin
#define verinHaut A2     //broche de montée du vérin
#define codeurRotatifA A3 // broche A du codeur rotatif
#define codeurRotatifB A4 // broche B du codeur rotatif


/*----------DEFINITION DES VARIABLES-----------*/

boolean etatCapteurFil = 0;       // état du capteur de fin de course : fil métallique
boolean etatCapteurVerin = 0;       // état du capteur de début de course : vérin
boolean etatCapteurLame = 0;       // état du capteur de fin de course : lame cisaille
boolean etat_bouton_marche_arret = 0; // état du bouton marche/arrêt
boolean dernier_etat_bouton_marche_arret = 0;// dernier état du bouton marche/arrêt

int etatCodeurRotatifA;// etat de la broche A du codeur rotatif
int etatCodeurRotatifB;// etat de la broche B du codeur rotatif
int dernierEtatCodeurRotatifA;// dernier etat de la broche A du codeur rotatif
int compteur = 0; // compteur
int compteurVoulu = 0; // nombre d'unités voulues
int maxCompteur = 5000;// nombre maximum d'unités

String etape = "INITIALISATION"; // variable d'affichage d'état

uint8_t etat = attente ; // variable d'état de la machine: initialisation en état d'attente
uint8_t dernier_etat = attente ; // variable de stockage du dernier état pour les cas d'urgence

unsigned long millisActuel = 0;
unsigned long millisDernier = 0;


/*----------DEFINITION DE L'AFFICHEUR-----------*/

#include <LiquidCrystal.h> // appel à la librairie de l'afficheur
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);// définition des broches de connexion de l'afficheur


/*----------FONCTION DE CONDIGURATION DE DEMARRAGE-----------*/

void setup() {

  pinMode(boutonMarcheArret, INPUT);// initialisation de la broche du bouton de démarrage comme entrée
  pinMode(capteurFil, INPUT);// initialisation de la broche du capteur de fin de course du fil métallique comme entrée
  pinMode(capteurVerin, INPUT);// initialisation de la broche du capteur de début de course du vérin comme entrée
  pinMode(capteurLame, INPUT);// initialisation de la broche du capteur de fin de course de la lame cisaille comme entrée
  pinMode(moteur, OUTPUT);// initialisation de la broche du moteur d'alimentation comme sortie
  pinMode(verinHaut, OUTPUT);// initialisation de la broche de la montée du vérin comme sortie
  pinMode(verinBas, OUTPUT);// initialisation de la broche de la descente du vérin comme sortie

  lcd.begin(16, 2);// initialisation de l'afficheur
  lcd.setCursor(0, 0);// positionnement du curseur
  lcd.print("PFE  Med JARRAYA");// affichage
  lcd.setCursor(3, 1);
  lcd.print("SOMIF 2017");
  delay(5000);// délais de 5 secondes
  lcd.clear();// réinitialisation de l'afficheur
  lcd.setCursor(1, 0);
  lcd.print(etape);
  delay(3000);// délais de 3 secondes
  lcd.clear();// réinitialisation de l'afficheur
  affichage();// appel de la fonction d'affichage
  affichage_MAJ(12, 0, String(compteur));
  affichage_MAJ(12, 1, String(compteurVoulu));
  dernierEtatCodeurRotatifA = digitalRead(codeurRotatifA); //sauvegarde du dernier état du codeur rotatif
}


/*----------FONCTION BOUCLE-----------*/

void loop() {

  temps ();
  marche_arret(); // appel de la fonction marche/arrêt
  machine_a_etats_finis();// appel de la fonction de détermination d'état de la machine
  
}


/*----------BLOCK DES FONCTIONS UTILITAIRES -----------*/
void temps () {

  millisActuel = millis();
  if (millisActuel - millisDernier >= 1000) {
    millisDernier = millisActuel;
    affichage_MAJ(2, 0, String(millis() / 1000));
  }
  
}

void nombre_d_unite() {// fonction de détermination du nombre d'unités à produire

  etatCodeurRotatifA = digitalRead(codeurRotatifA);
  etatCodeurRotatifB = digitalRead(codeurRotatifB);
  
  if (etatCodeurRotatifA != dernierEtatCodeurRotatifA) {
    if (etatCodeurRotatifB != etatCodeurRotatifA) {
      if (compteurVoulu < maxCompteur) {
        compteurVoulu += 10; //pas = 10 unités
      }
    } else {
      if (compteurVoulu > 0) {
        compteurVoulu -= 10;
      }
    }
    dernierEtatCodeurRotatifA = etatCodeurRotatifA;
    affichage_MAJ(12, 1, String(compteurVoulu));
  }
  
}

void affichage_MAJ(int pos, int li, String ch) {// fonction d'affichage des variables

  lcd.setCursor(pos, li);
  switch (pos) {
    case 2 :
      lcd.print("     ");
      break;
    case 12 :
      lcd.print("    ");
      break;
  }
  lcd.setCursor(pos, li);
  lcd.print(ch);

}


void affichage() { // fonction d'affichage: temps + nombre d'unités produites + état de la machine

  lcd.setCursor(0, 0);// positionnement du curseur
  lcd.print("T:");// affichage

  lcd.setCursor(8, 0);
  lcd.print("NbU:");

  lcd.setCursor(8, 1);
  lcd.print("NbM:");

}

void marche_arret() {// fonction de marche/arrêt: pour mettre en pause la machine en cas d'urgence

  etat_bouton_marche_arret = digitalRead(boutonMarcheArret);
  etatCapteurLame = digitalRead(capteurLame);
  etatCapteurFil = digitalRead(capteurFil);
  etatCapteurVerin = digitalRead(capteurVerin);

  if (etat_bouton_marche_arret != dernier_etat_bouton_marche_arret ) {
    if (etat_bouton_marche_arret == HIGH) {

      if (etat == attente && compteurVoulu > 0) {
        etat = 1 + (dernier_etat % 3);
      } else {
        etat = attente;
      }
    }
  }
  dernier_etat_bouton_marche_arret = etat_bouton_marche_arret;
  
}


void machine_a_etats_finis() {// fonction de détermination d'état de la machine

  // en foction de la valeur de la variable état, les actions à effectuer par la machine vont différer:
  switch (etat) {
    case attente :// en cas d' "attente"
      digitalWrite(moteur, LOW); //désactivation du moteur d'alimentation en fil métallique
      digitalWrite(verinHaut, LOW);//désactivation de la montée du vérin
      digitalWrite(verinBas, LOW);//désactivation de la déscente du vérin
      etape = "Attente";//changement de la valeur de la variable d'affichage
      affichage_MAJ( 0, 1, etape);
      nombre_d_unite();
      break;

    // ceci se répète pour les différents états de la machine

    case alimentation_en_fil :
      digitalWrite(moteur, HIGH);
      digitalWrite(verinHaut, LOW);
      digitalWrite(verinBas, LOW);
      affichage_MAJ( 0, 1, etape);
      etape = "Alm.FIL";
      if (etatCapteurFil == HIGH) {
        dernier_etat = etat;
        etat = descente_verin;

      }
      break;

    case descente_verin :
      digitalWrite(moteur, LOW);
      digitalWrite(verinHaut, LOW);
      digitalWrite(verinBas, HIGH);
      affichage_MAJ( 0, 1, etape);
      etape = "Dsc.VER";
      if (etatCapteurLame == HIGH) {
        compteur += 1;//incrémentation du compteur d'unités produites
        affichage_MAJ(12, 0, String(compteur));
        dernier_etat = etat;
        etat = montee_verin;

      }
      break;

    case montee_verin :
      digitalWrite(moteur, LOW);
      digitalWrite(verinHaut, HIGH);
      digitalWrite(verinBas, LOW);
      etape = "Mnt.VER";
      affichage_MAJ( 0, 1, etape);
      if (etatCapteurVerin == HIGH) {
        dernier_etat = etat;
        if (compteur < compteurVoulu) {
          etat = alimentation_en_fil;
        } else {
          compteurVoulu = 0;
          compteur = 0;
          affichage_MAJ(12, 0, String(compteur));
          affichage_MAJ(12, 1, String(compteurVoulu));
          etat = attente;
        }


      }
      break;
  }
}
