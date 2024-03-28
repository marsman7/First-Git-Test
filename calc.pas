unit IntegerTerm;

interface

uses SysUtils;

type
  VarRec = record
    Name: string;
    Wert: variant;
  end;
  VarArray = array of VarRec;
  PVarArray = ^VarArray;

  TIntTerm = class
    constructor Create();
  private
    { Private-Deklarationen }
    pVari: PVarArray;
    FehlerPos: integer;
    FehlerLaenge: integer;
    FehlerText: string;
    procedure FindeOperation(Term, Op: string; pPos: pinteger; p: integer);
    function  TermZerlegung(Term: string; p: integer): integer;
    procedure SetFehlerMeldung(s: string; p: integer; l: integer);
  public
    { Public-Deklarationen }
    function  TermLoesen(Term: string; pVariablen: PVarArray): integer;
    function GetFehler(FPos, FLaenge: pinteger): integer;
    function GetFehlerText: string;
  end;

const
  // Zeichen bzw. Symbole für Rechenoperation
  //    << Bits links schieben, >> Bits rechts schieben
  //    & AND, | OR, ~ XOR
  OpStr: array[0..8] of string = ('+','-','*','/','>>','<<','&','|','~');

implementation

constructor TIntTerm.Create;
begin
  pVari := nil;
  FehlerPos := 0;
  FehlerLaenge := 0;
  FehlerText := '';
end;

function TIntTerm.TermLoesen(Term: string; pVariablen: PVarArray): integer;
begin
  FehlerPos := 0;
  FehlerText := '';
  pVari := pVariablen;
  result := TermZerlegung(Term, 1);
end;

// -----------------------------------------------------------------------------
//  FindeOperation
//
//  Sucht die Rechenopertion, und gibt die Position zurück. In Klammern wird
//  nicht gesucht.
//
//  Term : Term, der nach einer Operation/Verknüpfung durchsucht werden soll
//  Op : Operation/Verknüpfung nach der in Term gesucht werden soll
//  pPos : Rückgabe der Position an der die Operation gefunden wurde
//  p : Zeichenposition im gesamt Term. Dient der Fehlerausgabe
// -----------------------------------------------------------------------------
procedure TIntTerm.FindeOperation(Term: string; Op: string; pPos: pinteger; p: integer);
var
  n: integer;     // Zeichenzähler
  k: integer;     // Klammerzähler, '(' plus 1 ; ')' minus 1
  kPos: integer;  // Merker für Position der ersten Klammer (Fehlerausgabe)
begin
  kPos := 0;
  k := 0;
  for n := 1 to length(Term) do
  begin
    if term[n] = '(' then
    begin
      if k = 0 then
        kPos := n;
      inc(k);
    end;
    if term[n] = ')' then
    begin
      dec(k);
      if k < 0 then
      begin
        SetFehlerMeldung('Klammer nicht geöffnet.', p+n-1, 1);
        exit;
      end;
    end;
    // if (k = 0) and (term[n] = Op) then
    if (k = 0) and (copy(term, n, length(Op)) = Op) then
    begin
      pPos^ := n;
      exit;
    end;
  end;
  pPos^ := 0;
  if k > 0 then
    SetFehlerMeldung('Öffnende Klammer gefunden, die nicht geschlossen wird.', p+kPos-1, 1);
end;
// ^^ FindeOperation ^^^^^^^^^^^^^^^^^^^^^^^^^^

// -----------------------------------------------------------------------------
// TermZerlegung
//
// Teilt den Term in seine Bestandteile auf
//
// Term : String des Term der entschlüsselt werden soll
// p : Zeichenposition im gesamt Term. Dient der Fehlerausgabe
// -----------------------------------------------------------------------------
function TIntTerm.TermZerlegung(Term: string; p: integer): integer;
var
  i: integer;
  n: integer;
  o1: integer;
  o2: integer;
  TempStr: string;
begin
  result := 0;

  if FehlerPos > 0 then
  begin
    result := 1;
    exit;
  end;

  p := p + length(Term) - length(TrimLeft(Term));
  Term := trim(Term);

  i := 0;
  while (i <= high(OpStr)) do
  begin
    FindeOperation(Term, OpStr[i], @n, p);
    if n > 0 then
    begin
      o1 := TermZerlegung(copy(Term, 1, n-1), p);
      o2 := TermZerlegung(copy(Term, n+length(OpStr[i]), length(Term)-n), p+n+length(OpStr[i])-1);
      case i of
        0: result := o1 + o2;
        1: result := o1 - o2;
        2: result := o1 * o2;
        3: result := o1 div o2;
        4: result := o1 shr o2;
        5: result := o1 shl o2;
        6: result := o1 and o2;
        7: result := o1 or o2;
        8: result := o1 xor o2;
      end;
      exit;
    end;
    inc(i);
  end;

  // Ist Term in Klammern
  if Term[1] = '(' then
  begin
    if Term[length(Term)] = ')' then
    begin
      result := TermZerlegung(trim(copy(Term, 2, length(Term)-2)), p+1);
      exit;
    end;
  end;

  // Auf Variable oder Funktion prüfen
  if Term[1] in ['a'..'z','A'..'Z','_'] then
  begin
    i := 2;
    while ((i <= length(Term)) and (Term[1] in ['a'..'z','A'..'Z','_','0'..'9'])) do
    begin
      inc(i);
    end;
    TempStr := copy(Term, 1, i-1);

    // Auf Variablen prüfen
    if assigned(pVari) then
    begin
      for n := low(pVari^) to high(pVari^) do
      begin
        if lowercase(pVari^[n].Name) = lowercase(TempStr) then
        begin
          result := pVari^[n].Wert;
          exit;
        end;
      end;
    end;
    SetFehlerMeldung('Syntax Fehler, Variable "'+TempStr+'" unbekannt.', p, length(TempStr));

    // Auf Funktionen prüfen (z.B. Not, sin, cos)
  end;

  // Wenn Term eine Zahl ist
  if Term[1] in ['0'..'9'] then
  begin
    try
      result := StrToInt(Term);
    except
      SetFehlerMeldung('Syntax Fehler, "'+Term+'" ist keine gültige Zahl.', p, length(Term));
    end;
    exit;
  end;
end;
// ^^ TermZerlegung ^^^^^^^^^^^^^^^^^^^^^^^^^^^

procedure TIntTerm.SetFehlerMeldung(s: string; p: integer; l: integer);
begin
  FehlerPos := p;
  FehlerLaenge := l;
  FehlerText := s;
  raise Exception.Create(s);
end;

function TIntTerm.GetFehler(FPos: pinteger; FLaenge: pinteger): integer;
begin
  result := 0;
  FPos^ := FehlerPos;
  FLaenge^ := FehlerLaenge;
  if FehlerPos > 0 then
    result := -1;
end;

function TIntTerm.GetFehlerText(): string;
begin
  result := FehlerText;
end;


{
procedure TForm1.Button1Click(Sender: TObject);
var
  test: VarArray;
begin
  SetLength(test, 4);
  test[0].Name := 'ertz';
  test[0].Wert := 25;
  test[1].Name := 's34';
  test[1].Wert := 3;
  test[2].Name := 'w3tz';
  test[2].Wert := 18;
  test[3].Name := 'w';
  test[3].Wert := 9;
  Memo1.Clear;
  Memo1.Lines.Add(format('Ergebnis = %d',[TermLoesen(Edit1.Text, @test)]));
end;
}

end.