// Funktionen zum berechnen des Ergebnis

#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#define OP_NUMBER   9

uint16_t FehlerPos;     //: integer;
uint16_t FehlerLaenge;  //: integer;
char FehlerText[512] = {0};     //: string;
void *pVari = 0;     //: PVarArray;

// Zeichen bzw. Symbole für Rechenoperation
//    << Bits links schieben, >> Bits rechts schieben
//    & AND, | OR, ~ XOR
const char *OpStr[OP_NUMBER] = {"+","-","*","/",">>","<<","&","|","~"};      //: array[0..8] of string = ('+','-','*','/','>>','<<','&','|','~');

void find_operation(char *term, char *Op, uint16_t *pPos, uint16_t p);

/** ***************************************************************************
 * Name
 *
 * Discription
 *
 * @param a  Discription
 * @return no
 *************************************************************************** */
bool calc_term(char *term, int64_t *result)
{
    if (!term) return false;

    if (!strlen(term)) return false;

    printf("Trimmed text [%s]\n", term);

    *result = term_disassemble(term, 0, result);

    return true;
}


/** ***************************************************************************
 * TermZerlegung
 *
 * Teilt den Term in seine Bestandteile auf
 *
 * @param Term String des Term der entschlüsselt werden soll
 * @param p    Zeichenposition im gesamt Term. Dient der Fehlerausgabe
 * @return Ergebnis
 *************************************************************************** */
bool term_disassemble(char *term, uint16_t p, int64_t *result) 
{
    char *pPos = term;         // Zeiger auf Position die verarbeitet wird
    char *pEnd = term + strlen(term);   // Zeiger auf Position die verarbeitet wird
    int32_t i = 0;      //: integer;
    uint32_t n;         //: integer;
    uint64_t left_opp;        //left operant : integer;
    uint64_t right_opp;        //right operant : integer;
    char sTempStr[512] = {0}; //: string;

    uint64_t tempresult = 0;

    if (FehlerPos > 0) {
        return false;
    }

    // Trim left
    while ((*pPos) && isspace(*pPos)) {
        pPos++;
    }

    // Trim right
    while(pEnd > pPos) {
        pEnd--;
        if (isspace(*pEnd)) {
            *(pEnd) = '\0';
        }
    }

#warning Hier weiter machen

//    p = p + strlen(term) - strlen(TrimLeft(term));
//    term = trim(term);

    i = 0;
    while (i < OP_NUMBER) {
        find_operation(pPos, OpStr[i], &n, p);
        if (n > 0) {
//        left_opp = TermZerlegung(copy(term, 1, n-1), p);
//          right_opp = TermZerlegung(copy(term, n+strlen(OpStr[i]), strlen(term)-n), p+n+strlen(OpStr[i])-1);

            term_disassemble(term, p, &left_opp); 
            term_disassemble(term, p, &right_opp); 

            switch (i) {
                case 0: tempresult = left_opp + right_opp; break;
                case 1: tempresult = left_opp - right_opp; break;
                case 2: tempresult = left_opp * right_opp; break;
                case 3: tempresult = left_opp / right_opp; break;
                case 4: tempresult = left_opp >> right_opp; break;
                case 5: tempresult = left_opp << right_opp; break;
                case 6: tempresult = left_opp & right_opp; break;
                case 7: tempresult = left_opp | right_opp; break;
                case 8: tempresult = left_opp ^ right_opp; break;
                default:
                    ;
            }
            return result;
        }
        i++;
    }

    // Ist Term in Klammern
    if (term[1] == '(') {
        if (term[strlen(term)] == ')') {
        result = TermZerlegung(trim(copy(term, 2, strlen(term)-2)), p+1);
        return result;
        }
    }

    // Auf Variable oder Funktion prüfen
    if (isalpha(term[1]) || term[1] == '_') {
        i = 2;
        while ((i <= strlen(term)) & (isalnum(term[1]) || term[1] == '_')) {
            i++;
        }
        strncpy(sTempStr,term, i-1);    // sTempStr = copy(term, 1, i-1);

/* 
    // Auf Variablen prüfen
    if (assigned(pVari)) {
      for (n := low(pVari^) to high(pVari^)) {
        if (lowercase(pVari^[n].Name) = lowercase(sTempStr)) {
          result = pVari^[n].Wert;
          return result;
        }
      }
    }
*/
        SetFehlerMeldung('Syntax Fehler, Variable "'+sTempStr+'" unbekannt.', p, strlen(sTempStr));

        // Auf Funktionen prüfen (z.B. Not, sin, cos)
    }

    // Wenn Term eine Zahl ist
    if (term[1] in ['0'..'9']) {
        try
        result := StrToInt(term);
        except
        SetFehlerMeldung('Syntax Fehler, "'+term+'" ist keine gültige Zahl.', p, strlen(term));
        end;
    }

    return result;
}
// ^^ TermZerlegung ^^^^^^^^^^^^^^^^^^^^^^^^^^^

/** ***************************************************************************
 * find_operation
 *
 * Sucht die Rechenopertion, und gibt die Position zurück. In Klammern wird
 * nicht gesucht.
 *
 * @param Term  Term, der nach einer Operation/Verknüpfung durchsucht werden soll
 * @param Op    Operation/Verknüpfung nach der in Term gesucht werden soll
 * @param pPos  Rückgabe der Position an der die Operation gefunden wurde
 * @param p     Zeichenposition im gesamt Term. Dient der Fehlerausgabe
 * @return no
 *************************************************************************** */
void find_operation(char *term, char *Op, uint16_t *pPos, uint16_t p)
{
  uint16_t n = 0;   //: integer;     // Zeichenzähler
  uint16_t k = 0;   //: integer;     // Klammerzähler, '(' plus 1 ; ')' minus 1
  uint16_t kPos = 0;    //: integer;  // Merker für Position der ersten Klammer (Fehlerausgabe)

  for (n = 1 to strlen(term)) {
    if (term[n] == '(') {
      if (k == 0) {
        kPos = n;
      }
      k++;
    }
    if (term[n] == ')') {
      dec(k);
      if (k < 0) {
        SetFehlerMeldung('Klammer nicht geöffnet.', p+n-1, 1);
        return;
      }
    }
    // if (k = 0) and (term[n] = Op) then
    if ((k = 0) & (copy(term, n, length(Op)) = Op)) {
      *pPos = n;
      return;
    }
  }

  *pPos = 0;
  if (k > 0) {
    SetFehlerMeldung('Öffnende Klammer gefunden, die nicht geschlossen wird.', p+kPos-1, 1);
  }
}
// ^^ find_operation ^^^^^^^^^^^^^^^^^^^^^^^^^^

/** ***************************************************************************
 * Name
 *
 * Discription
 *
 * @param a  Discription
 * @return no
 *************************************************************************** */
void SetFehlerMeldung(const char *s, uint16_t p, uint16_t l) {
  FehlerPos = p;
  FehlerLaenge = l;
  FehlerText = s;
}

/** ***************************************************************************
 * Name
 *
 * Discription
 *
 * @param a  Discription
 * @return no
 *************************************************************************** */
int16_t GetFehler(uint16_t *FPos, uint16_t *FLaenge) {
  int16_t result = 0;
  *FPos = FehlerPos;
  *FLaenge = FehlerLaenge;
  if (FehlerPos > 0) {
    result = -1;
  }

  return result;
}

/** ***************************************************************************
 * Name
 *
 * Discription
 *
 * @param a  Discription
 * @return no
 *************************************************************************** */
const char *GetFehlerText() {
  const char *result = FehlerText;
  return result;
}
