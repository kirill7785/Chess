unit Unit1;
// Шахматы в Лазаре v2.2.4
// 4.01.2023 начало 733 строки кода.
// Взятие на проходе.
// Откад хода назад (отмена одного хода Undo). Не забыть вернуть сьеденную фигуру.
// Внимание не более двух undo иначе неопределенное поведение.
// рокировка.
// Подсвечивания при шахе красным.
// Сохранение партии в файл. Save; Open; TODO
// Воспроизведение только что сыгранной партии. TODO
// 5.01.2023 физические шахматы. 3569 строк кода.
// 6.01.2023 исправление ошибок 3863 строки кода.
// 7.01.2023 Удалил пешку при взятии на проходе. Исправлено.

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, ExtCtrls,
  Menus;

// 0 - empty, 1 - king, 2 - queen, 3 - rook, 4 - bishop,  5 - knight, 6 - pawn;
// 0 - empty, 1 - black, 2 - white;
 const
       cemptyfig=0;
       cking=1;
       cqueen=2;
       crook=3;
       cbishop=4;
       cknight=5;
       cpawn=6;

       cempty=0;
       cblack=1;
       cwhite=2;


type

  { TForm1 }

  Position = record
    i,j : Integer;
  end;

  Move = record
    from, to_ : Position;
    fig : Integer;
  end;

  Figure = record
    i,j,fig : Integer;
  end;

  TForm1 = class(TForm)
    MainMenu1: TMainMenu;
    MenuItem1: TMenuItem;
    MenuItem2: TMenuItem;
    MenuItem3: TMenuItem;
    Timer1: TTimer;
    procedure FormCreate(Sender: TObject);
    procedure FormMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure FormPaint(Sender: TObject);
    procedure MenuItem3Click(Sender: TObject);

  private
    { private declarations }
  public
    { public declarations }
    xmouse, ymouse : Integer;
    bPress : Boolean;
    arrw : array of Figure;
    arrb : array of Figure;
    cMove : Integer; // Чей ход белых или чёрных ?
    vacantmove : array[1..8,1..8] of Boolean; // Подсвечиваем клетки куда можно пойти по правилам Шахмат.
    vacantmove1 : array[1..8,1..8] of Boolean;
    white_previos_move, black_previos_move : Move; // Предыдущий ход забелых и за чёрных.
    arrw1 : array of Figure;  // Состояние доски на предыдущем ходу.
    arrb1 : array of Figure;
    white_previos_move1, black_previos_move1 : Move; // Предыдущий ход забелых и за чёрных.
    // Массивы attack заполняются по требованию.
    white_attack : array[1..8,1..8] of Boolean; // Поля в которых атакуют белые фигуры
    black_attack : array[1..8,1..8] of Boolean; // Поля в которых атакуют чёрные фигуры
    arrwdetector : array of Figure; // Используется для детектора шахов.
    arrbdetector : array of Figure; // Заполняется по требованию.
    did_the_white_right_rook_move, did_the_white_left_rook_move : Boolean; // ходили ли белые ладьи
    did_the_black_right_rook_move, did_the_black_left_rook_move : Boolean; // ходили ли чёрные ладьи
    did_the_white_king_move, did_the_black_king_move : Boolean; // ходили ли короли.
    did_the_white_right_rook_move1, did_the_white_left_rook_move1 : Boolean; // ходили ли белые ладьи
    did_the_black_right_rook_move1, did_the_black_left_rook_move1 : Boolean; // ходили ли чёрные ладьи
    did_the_white_king_move1, did_the_black_king_move1 : Boolean; // ходили ли короли.
    white_eating, black_eating : Figure; // Сьеденная фигура которую нужно вернуть при UNDO.
    State : Integer; // Не более двух нажатий undo.

   procedure Kletka(x,y,a:integer;cl:TColor);
   procedure Pawn(x,y,a:integer;c1,c2:TColor);   // Нарисовать пешку
   procedure Rook(x,y,a:integer;c1,c2:TColor);   // Нарисовать ладью
   procedure King(x,y,a:integer;c1,c2:TColor);   // Нарисовать короля
   procedure Queen(x,y,a:integer;c1,c2:TColor);  // Нарисовать королеву
   procedure Bishop(x,y,a:integer;c1,c2:TColor); // Нарисовать слона
   procedure Knight(x,y,a:integer;c1,c2:TColor); // Нарисовать коня
   procedure Draw(Sender: TObject);  // нарисовать всё.
   procedure DeleteFig(i0,j0 : Integer); // удалить фигуру из списка фигур по заданным координатам
   procedure DeleteFig1(i0,j0 : Integer); // удалить фигуру из списка фигур по заданным координатам
   procedure DeleteFigdetector(i0,j0 : Integer);
   function WhotisPole(i0,j0 : Integer): Integer;
   function WhotisPoledetector(i0,j0 : Integer): Integer;   // Для копии доски.
   function WhotisFig(i0,j0 : Integer): Integer; // какая фигура стоит на поле i0,j0 ?
   procedure GenerateWhiteAttack(); // Генерирует поля которые бьют белые фигуры.
   procedure GenerateBlackAttack(); // Генерирует поля которые бьют чёрные фигуры.
   procedure CopyList(); // Копирование фигур для детектора шахов.
   function WhiteKing_Check(Sender : TObject) : Boolean; // Белый король под Шахом ?
   function BlackKing_Check(Sender : TObject) : Boolean; // Чёрный король под Шахом ?
   procedure DrawBlackAttack(Sender : TObject);
   procedure DrawWhiteAttack(Sender : TObject);
   function MatorPat(Sender : TObject) : Boolean; // Возвращает true если некуда идти.
  end;

var
  Form1: TForm1;

implementation

{$R *.lfm}

{ TForm1 }

  // Динамическое выделение памяти почему то дает сбой, я не понял почему
// и сделал вместо удаления пометку заглушка cemptyfig.
{
procedure TForm1.DeleteFig(i0,j0 : Integer);
var k,k_1, ic : Integer;
    found : Boolean;
    tmp :   array of Figure;

begin
   found:=false;
   k_1:=0;
   ic:=0;
   tmp:=nil;
   SetLength(tmp,Length(arrw));
   for k:=0 to High(arrw) do
   begin // initialization
      tmp[k].i:=-1;
      tmp[k].j:=-1;
      tmp[k].fig:=cemptyfig;
   end;
   for k:=0 to High(arrw) do
   begin
      if (not((arrw[k].i=i0)and(arrw[k].j=j0))) then
      begin
         tmp[k_1]:=arrw[k];
         inc(k_1);
      end
      else
      begin
         found:=true;
         inc(ic);
      end;
   end;

   if (found) then
   begin
      SetLength(arrw,k_1);
      for k:=0 to High(arrw) do
      begin
         arrw[k]:=tmp[k];
      end;
   end;
   SetLength(tmp,0);
   tmp:=nil;

   found:=false;
   k_1:=0;
   ic:=0;
   SetLength(tmp,Length(arrb));
    for k:=0 to High(arrb) do
   begin // initialization
      tmp[k].i:=-1;
      tmp[k].j:=-1;
      tmp[k].fig:=cemptyfig;
   end;
   for k:=0 to High(arrb) do
   begin
      if (not((arrb[k].i=i0)and(arrb[k].j=j0))) then
      begin
         tmp[k_1]:=arrb[k];
         inc(k_1);
      end
      else
      begin
         found:=true;
         inc(ic);
      end;
   end;

   if (found) then
   begin
      SetLength(arrb,k_1);
      for k:=0 to High(arrb) do
      begin
         arrb[k]:=tmp[k];
      end;
   end;
   SetLength(tmp,0);
   tmp:=nil;
   Finalize(tmp);
end;



 procedure TForm1.DeleteFig1(i0,j0 : Integer);
var k,k_1, ic : Integer;
    found : Boolean;
    tmp :   array of Figure;

begin
   found:=false;
   k_1:=0;
   ic:=0;
   tmp:=nil;
   SetLength(tmp,Length(arrw1));
   for k:=0 to High(arrw1) do
   begin // initialization
      tmp[k].i:=-1;
      tmp[k].j:=-1;
      tmp[k].fig:=cemptyfig;
   end;
   for k:=0 to High(arrw1) do
   begin
      if (not((arrw1[k].i=i0)and(arrw1[k].j=j0))) then
      begin
         tmp[k_1]:=arrw1[k];
         inc(k_1);
      end
      else
      begin
         found:=true;
         inc(ic);
      end;
   end;

   if (found) then
   begin
      SetLength(arrw1,k_1);
      for k:=0 to High(arrw1) do
      begin
         arrw1[k]:=tmp[k];
      end;
   end;
   SetLength(tmp,0);
   tmp:=nil;

   found:=false;
   k_1:=0;
   ic:=0;
   SetLength(tmp,Length(arrb1));
    for k:=0 to High(arrb1) do
   begin // initialization
      tmp[k].i:=-1;
      tmp[k].j:=-1;
      tmp[k].fig:=cemptyfig;
   end;
   for k:=0 to High(arrb1) do
   begin
      if (not((arrb1[k].i=i0)and(arrb1[k].j=j0))) then
      begin
         tmp[k_1]:=arrb1[k];
         inc(k_1);
      end
      else
      begin
         found:=true;
         inc(ic);
      end;
   end;

   if (found) then
   begin
      SetLength(arrb1,k_1);
      for k:=0 to High(arrb1) do
      begin
         arrb1[k]:=tmp[k];
      end;
   end;
   SetLength(tmp,0);
   tmp:=nil;
   Finalize(tmp);
end;


// Для детектора шахов.
procedure TForm1.DeleteFigdetector(i0,j0 : Integer);
var k, k_1, ic : Integer;
    found : Boolean;
    tmp :   array of Figure;

begin
   found:=false;
   k_1:=0;
   ic:=0;
   tmp:=nil;
   SetLength(tmp,Length(arrwdetector));
   for k:=0 to High(arrwdetector) do
   begin // initialization
      tmp[k].i:=-1;
      tmp[k].j:=-1;
      tmp[k].fig:=cemptyfig;
   end;
   for k:=0 to High(arrwdetector) do
   begin
      if (not((arrwdetector[k].i=i0)and(arrwdetector[k].j=j0))) then
      begin
         tmp[k_1]:=arrwdetector[k];
         inc(k_1);
      end
      else
      begin
         found:=true;
         inc(ic);
      end;
   end;

   if (found) then
   begin
      SetLength(arrwdetector,k_1);
      for k:=0 to High(arrwdetector) do
      begin
         arrwdetector[k]:=tmp[k];
      end;
   end;
   SetLength(tmp,0);
   tmp:=nil;

   found:=false;
   k_1:=0;
   ic:=0;
   SetLength(tmp,Length(arrbdetector));
    for k:=0 to High(arrbdetector) do
   begin // initialization
      tmp[k].i:=-1;
      tmp[k].j:=-1;
      tmp[k].fig:=cemptyfig;
   end;
   for k:=0 to High(arrbdetector) do
   begin
      if (not((arrbdetector[k].i=i0)and(arrbdetector[k].j=j0))) then
      begin
         tmp[k_1]:=arrbdetector[k];
         inc(k_1);
      end
      else
      begin
         found:=true;
         inc(ic);
      end;
   end;

   if (found) then
   begin
      SetLength(arrbdetector,k_1);
      for k:=0 to High(arrbdetector) do
      begin
         arrbdetector[k]:=tmp[k];
      end;
   end;
   SetLength(tmp,0);
   tmp:=nil;
   Finalize(tmp);
end;
 }


function TForm1.MatorPat(Sender : TObject) : Boolean; // Возвращает true если некуда идти.
//var
  //  i,j : Integer;
    //flag : Boolean;
begin
  { flag:=false;
   for i:=1 to 8 do
   for j:=1 to 8 do
   if (vacantmove[i,j]) then flag:=true;

   if (flag) then
   begin
      Result:=false;
   end
    else
   begin
      Result:=true; // Некуда идти

   end;}
    // Чтобы правильно детектировать что некуда ходить нужно
    // перебрать все фигуры а не только ту по которой пользователь
    // Щёлкнул мышкой.
    // метод не работает 06.01.2023.
    Result:=false;
end;


procedure TForm1.DeleteFig(i0,j0 : Integer);
var k: Integer;
begin


   for k:=0 to High(arrw) do
   begin
      if (((arrw[k].fig<>cemptyfig)and(arrw[k].i=i0)and(arrw[k].j=j0))) then
      begin
          arrw[k].fig:=cemptyfig;
          arrw[k].i:=-1;
          arrw[k].j:=-1;
      end
   end;

   for k:=0 to High(arrb) do
   begin
      if (((arrb[k].fig<>cemptyfig)and(arrb[k].i=i0)and(arrb[k].j=j0))) then
      begin
          arrb[k].fig:=cemptyfig;
          arrb[k].i:=-1;
          arrb[k].j:=-1;
      end
   end;
end;



 procedure TForm1.DeleteFig1(i0,j0 : Integer);
var k : Integer;
begin
    for k:=0 to High(arrw1) do
   begin
      if (((arrw1[k].fig<>cemptyfig)and(arrw1[k].i=i0)and(arrw1[k].j=j0))) then
      begin
          arrw1[k].fig:=cemptyfig;
          arrw1[k].i:=-1;
          arrw1[k].j:=-1;
      end
   end;

   for k:=0 to High(arrb1) do
   begin
      if (((arrb1[k].fig<>cemptyfig)and(arrb1[k].i=i0)and(arrb1[k].j=j0))) then
      begin
          arrb1[k].fig:=cemptyfig;
          arrb1[k].i:=-1;
          arrb1[k].j:=-1;
      end
   end;
end;


// Для детектора шахов.
procedure TForm1.DeleteFigdetector(i0,j0 : Integer);
var k : Integer;


begin
  for k:=0 to High(arrwdetector) do
   begin
      if (((arrwdetector[k].fig<>cemptyfig)and(arrwdetector[k].i=i0)and(arrwdetector[k].j=j0))) then
      begin
          arrwdetector[k].fig:=cemptyfig;
          arrwdetector[k].i:=-1;
          arrwdetector[k].j:=-1;
      end
   end;

   for k:=0 to High(arrbdetector) do
   begin
      if (((arrbdetector[k].fig<>cemptyfig)and(arrbdetector[k].i=i0)and(arrbdetector[k].j=j0))) then
      begin
          arrbdetector[k].fig:=cemptyfig;
          arrbdetector[k].i:=-1;
          arrbdetector[k].j:=-1;
      end
   end;
end;



function TForm1.WhotisPole(i0,j0 : Integer) : Integer;
var k : Integer;
    found : Boolean;
begin
   found:=false;

   for k:=0 to High(arrw) do
   begin
      if ((arrw[k].i=i0)and(arrw[k].j=j0)) then
      begin
         found:=true;
      end;
   end;

   if (found) then
   begin
       Result:=cwhite;
   end
   else
   begin
      found:=false;

      for k:=0 to High(arrb) do
      begin
         if ((arrb[k].i=i0)and(arrb[k].j=j0)) then
         begin
            found:=true;
         end;
      end;

      if (found) then
      begin
         Result:=cblack;
      end
       else
      begin
         Result:=cempty;
      end;
   end;
end;

function TForm1.WhotisPoledetector(i0,j0 : Integer) : Integer;
var k : Integer;
    found : Boolean;
begin
   found:=false;

   for k:=0 to High(arrwdetector) do
   begin
      if ((arrwdetector[k].i=i0)and(arrwdetector[k].j=j0)) then
      begin
         found:=true;
      end;
   end;

   if (found) then
   begin
       Result:=cwhite;
   end
   else
   begin
      found:=false;

      for k:=0 to High(arrbdetector) do
      begin
         if ((arrbdetector[k].i=i0)and(arrbdetector[k].j=j0)) then
         begin
            found:=true;
         end;
      end;

      if (found) then
      begin
         Result:=cblack;
      end
       else
      begin
         Result:=cempty;
      end;
   end;
end;

function TForm1.WhotisFig(i0,j0 : Integer) : Integer;
var k, fig : Integer;
    found : Boolean;
begin
   found:=false;

   for k:=0 to High(arrw) do
   begin
      if ((arrw[k].i=i0)and(arrw[k].j=j0)) then
      begin
         found:=true;
         fig:=arrw[k].fig;
      end;
   end;

   if (found) then
   begin
       Result:=fig;
   end
   else
   begin
      found:=false;

      for k:=0 to High(arrb) do
      begin
         if ((arrb[k].i=i0)and(arrb[k].j=j0)) then
         begin
            found:=true;
            fig:=arrb[k].fig;
         end;
      end;

      if (found) then
      begin
         Result:=fig;
      end
       else
      begin
         Result:=cemptyfig;
      end;
   end;
end;

procedure TForm1.kletka(x,y,a:integer;cl:TColor);
begin
with Canvas do
 begin
  pen.Color:=cl;
  brush.Color:=cl;
  rectangle(x,y,x+a,y+a);
 end;
end;

procedure TForm1.Pawn(x,y,a:integer;c1,c2:TColor);
var xc,yc,s:integer;
  cmem1,cmem2 : TColor;
begin
xc:=x+a div 2;
yc:=y+a div 2;
s:=10;
with Canvas do
 begin
    cmem1:=Color;
    Color:=c2;
    cmem2:=font.Color;
    font.Color:=c1;
    textout(xc-13,yc-2*s,'_o_');
    textout(xc-s,yc,'(_)');
    font.Color:=cmem2;
    Color:=cmem1;
 end;
end;

procedure TForm1.Rook(x,y,a:integer;c1,c2:TColor);
var xc,yc,s:integer;
  cmem1,cmem2 : TColor;
begin
xc:=x+a div 2;
yc:=y+a div 2;
s:=10;
with Canvas do
 begin
    cmem1:=Color;
    Color:=c2;
    cmem2:=font.Color;
    font.Color:=c1;
    textout(xc-s,yc-2*s,'["]');
    textout(xc-s,yc,'(_)');
    font.Color:=cmem2;
    Color:=cmem1;
 end;
end;

procedure TForm1.King(x,y,a:integer;c1,c2:TColor);
var xc,yc,s:integer;
  cmem1,cmem2 : TColor;
begin
xc:=x+a div 2;
yc:=y+a div 2;
s:=10;
with Canvas do
 begin
    cmem1:=Color;
    Color:=c2;
    cmem2:=font.Color;
    font.Color:=c1;
    textout(xc-13,yc-2*s,'\+/');
    textout(xc-s,yc,'(_)');
    font.Color:=cmem2;
    Color:=cmem1;
 end;
end;

procedure TForm1.Queen(x,y,a:integer;c1,c2:TColor);
var xc,yc,s:integer;
  cmem1,cmem2 : TColor;
begin
xc:=x+a div 2;
yc:=y+a div 2;
s:=10;
with Canvas do
 begin
    cmem1:=Color;
    Color:=c2;
    cmem2:=font.Color;
    font.Color:=c1;
    textout(xc-13,yc-2*s,'\^/');
    textout(xc-s,yc,'(_)');
    font.Color:=cmem2;
    Color:=cmem1;
 end;
end;

procedure TForm1.Bishop(x,y,a:integer;c1,c2:TColor);
var xc,yc,s:integer;
  cmem1,cmem2 : TColor;
begin
xc:=x+a div 2;
yc:=y+a div 2;
s:=10;
with Canvas do
 begin
    cmem1:=Color;
    Color:=c2;
    cmem2:=font.Color;
    font.Color:=c1;
    textout(xc-s,yc-2*s,'(\)');
    textout(xc-s,yc,'(_)');
    font.Color:=cmem2;
    Color:=cmem1;
 end;
end;

procedure TForm1.Knight(x,y,a:integer;c1,c2:TColor);
var xc,yc,s:integer;
  cmem1,cmem2 : TColor;
begin
xc:=x+a div 2;
yc:=y+a div 2;
s:=10;
with Canvas do
 begin
    cmem1:=Color;
    Color:=c2;
    cmem2:=font.Color;
    font.Color:=c1;
    textout(xc-s,yc-2*s,'{o\');
    textout(xc-s,yc,'(_)');
    font.Color:=cmem2;
    Color:=cmem1;
 end;
end;

procedure TForm1.CopyList();
var
    i : Integer;
begin
   SetLength(arrwdetector,Length(arrw));
   SetLength(arrbdetector,Length(arrb));

   for i:=0 to High(arrw) do
   begin
      arrwdetector[i]:=arrw[i];
   end;
   for i:=0 to High(arrb) do
   begin
      arrbdetector[i]:=arrb[i];
   end;
end;

procedure TForm1.DrawBlackAttack(Sender : TObject);
var
    i,j : Integer;
begin
   for i:=1 to 8 do
   for j:=1 to 8 do
   begin
      vacantmove1[i][j]:=black_attack[i][j];
   end;
   //Draw(Sender);
   //sleep(1000);
end;

procedure TForm1.DrawWhiteAttack(Sender : TObject);
var
    i,j : Integer;
begin
   for i:=1 to 8 do
   for j:=1 to 8 do
   begin
      vacantmove1[i][j]:=white_attack[i][j];
   end;
   //Draw(Sender);
   //sleep(1000);
end;

function TForm1.WhiteKing_Check(Sender : TObject) : Boolean; // Белый король под Шахом ?
var
    i : Integer;
    Shah : Boolean;
begin

   Shah:=false;
   for i:=0 to High(arrwdetector) do
   begin
      if (arrwdetector[i].fig=cking) then
      begin
        if (black_attack[arrwdetector[i].i,arrwdetector[i].j]) then
        begin
           Shah:=true; // Под Шахом
        end;
      end;
   end;
   Result:=Shah;
end;

function TForm1.BlackKing_Check(Sender : TObject) : Boolean; // Чёрный король под Шахом ?
var
    i : Integer;
    Shah : Boolean;
begin
   Shah:=false;
   for i:=0 to High(arrbdetector) do
   begin
      if (arrbdetector[i].fig=cking) then
      begin
        if (white_attack[arrbdetector[i].i,arrbdetector[i].j]) then
        begin
           Shah:=true; // Под Шахом
        end;
      end;
   end;
   Result:=Shah;
end;

// Использует копию доски - detector.
procedure TForm1.GenerateWhiteAttack(); // Генерирует поля которые бьют белые фигуры.
var
  i,j, i_1, i_3, i_2, j_2 : Integer;
begin
for i:=1 to 8 do
for j:=1 to 8 do
     white_attack[i,j]:=false;

for i_1:=0 to High(arrwdetector) do
begin
   if (arrwdetector[i_1].fig<>cemptyfig) then
      begin


         i:=arrwdetector[i_1].i;
         j:=arrwdetector[i_1].j;

         if (arrwdetector[i_1].fig=cpawn) then
                          begin

                             // пешка ест наискосок.
                             if ((j>1)and(WhotisPoledetector(i-1,j-1)<>cwhite)) then  white_attack[i-1][j-1]:=true;
                             if ((j<8)and(WhotisPoledetector(i-1,j+1)<>cwhite)) then  white_attack[i-1][j+1]:=true;

                             // Взятие на проходе
                             {if ((black_previos_move.fig=cpawn)and
                             (black_previos_move.from.i=2) and
                             (black_previos_move.to_.i=4) and
                             (((j>1)and(black_previos_move.from.j=j-1))or
                             ((j<8)and(black_previos_move.from.j=j+1))) and
                             (i=4)) then
                             begin
                                if ((j>1)and(black_previos_move.from.j=j-1)) then
                                begin
                                   white_attack[i-1][j-1]:=true;
                                end
                                else
                                begin
                                   white_attack[i-1][j+1]:=true;
                                end;
                             end;}
                          end;
                          if (arrwdetector[i_1].fig=cknight) then
                          begin
                             if (i+2<=8)and(j+1<=8)and(WhotisPoledetector(i+2,j+1)<>cwhite) then white_attack[i+2][j+1]:=true;
                             if (i+1<=8)and(j+2<=8)and(WhotisPoledetector(i+1,j+2)<>cwhite) then white_attack[i+1][j+2]:=true;
                             if (i-2>=1)and(j-1>=1)and(WhotisPoledetector(i-2,j-1)<>cwhite) then white_attack[i-2][j-1]:=true;
                             if (i-1>=1)and(j-2>=1)and(WhotisPoledetector(i-1,j-2)<>cwhite) then white_attack[i-1][j-2]:=true;
                             if (i+2<=8)and(j-1>=1)and(WhotisPoledetector(i+2,j-1)<>cwhite) then white_attack[i+2][j-1]:=true;
                             if (i+1<=8)and(j-2>=1)and(WhotisPoledetector(i+1,j-2)<>cwhite) then white_attack[i+1][j-2]:=true;
                             if (i-2>=1)and(j+1<=8)and(WhotisPoledetector(i-2,j+1)<>cwhite) then white_attack[i-2][j+1]:=true;
                             if (i-1>=1)and(j+2<=8)and(WhotisPoledetector(i-1,j+2)<>cwhite) then white_attack[i-1][j+2]:=true;
                          end;
                          if (arrwdetector[i_1].fig=crook) then
                          begin
                             i_3:=j;
                             if (i_3<8) then
                             begin
                                while (i_3+1<=8) and (WhotisPoledetector(i,i_3+1)<>cwhite) do
                                begin
                                   inc(i_3);
                                   white_attack[i,i_3]:=true;
                                   if (WhotisPoledetector(i,i_3)=cblack) then break;
                                end;
                             end;
                              i_3:=j;
                             if (i_3>1) then
                             begin
                                while (i_3-1>=1) and (WhotisPoledetector(i,i_3-1)<>cwhite) do
                                begin
                                   dec(i_3);
                                   white_attack[i,i_3]:=true;
                                   if (WhotisPoledetector(i,i_3)=cblack) then break;
                                end;
                             end;
                             i_3:=i;
                             if (i_3<8) then
                             begin
                                while (i_3+1<=8) and (WhotisPoledetector(i_3+1,j)<>cwhite) do
                                begin
                                   inc(i_3);
                                   white_attack[i_3,j]:=true;
                                   if (WhotisPoledetector(i_3,j)=cblack) then break;
                                end;
                             end;
                               i_3:=i;
                             if (i_3>1) then
                             begin
                                while (i_3-1>=1) and (WhotisPoledetector(i_3-1,j)<>cwhite) do
                                begin
                                   dec(i_3);
                                   white_attack[i_3,j]:=true;
                                   if (WhotisPoledetector(i_3,j)=cblack) then break;
                                end;
                             end;
                          end;
                           if (arrwdetector[i_1].fig=cbishop) then
                          begin
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2<8) then
                             begin
                                while (i_2+1<=8)and(j_2+1<=8) and (WhotisPoledetector(i_2+1,j_2+1)<>cwhite) do
                                begin
                                   inc(i_2);
                                   inc(j_2);
                                   white_attack[i_2,j_2]:=true;
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2>1) then
                             begin
                                while (i_2-1>=1)and(j_2-1>=1) and (WhotisPoledetector(i_2-1,j_2-1)<>cwhite) do
                                begin
                                   dec(i_2);
                                   dec(j_2);
                                   white_attack[i_2,j_2]:=true;
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2<8) then
                             begin
                                while (i_2-1>=1) and (j_2+1<=8) and (WhotisPoledetector(i_2-1,j_2+1)<>cwhite) do
                                begin
                                   dec(i_2);
                                   inc(j_2);
                                   white_attack[i_2,j_2]:=true;
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2>1) then
                             begin
                                while (i_2+1<=8)and(j_2-1>=1) and (WhotisPoledetector(i_2+1,j_2-1)<>cwhite) do
                                begin
                                   inc(i_2);
                                   dec(j_2);
                                   white_attack[i_2,j_2]:=true;
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then break;
                                end;
                             end;
                          end;
                          if (arrwdetector[i_1].fig=cqueen) then
                          begin
                             // Королева - сочетание ладьи и слона.
                             i_3:=j;
                             if (i_3<8) then
                             begin
                                while (i_3+1<=8) and (WhotisPoledetector(i,i_3+1)<>cwhite) do
                                begin
                                   inc(i_3);
                                   white_attack[i,i_3]:=true;
                                   if (WhotisPoledetector(i,i_3)=cblack) then break;
                                end;
                             end;
                              i_3:=j;
                             if (i_3>1) then
                             begin
                                while (i_3-1>=1) and (WhotisPoledetector(i,i_3-1)<>cwhite) do
                                begin
                                   dec(i_3);
                                   white_attack[i,i_3]:=true;
                                   if (WhotisPoledetector(i,i_3)=cblack) then break;
                                end;
                             end;
                             i_3:=i;
                             if (i_3<8) then
                             begin
                                while (i_3+1<=8) and (WhotisPoledetector(i_3+1,j)<>cwhite) do
                                begin
                                   inc(i_3);
                                   white_attack[i_3,j]:=true;
                                   if (WhotisPoledetector(i_3,j)=cblack) then break;
                                end;
                             end;
                               i_3:=i;
                             if (i_3>1) then
                             begin
                                while (i_3-1>=1) and (WhotisPoledetector(i_3-1,j)<>cwhite) do
                                begin
                                   dec(i_3);
                                   white_attack[i_3,j]:=true;
                                   if (WhotisPoledetector(i_3,j)=cblack) then break;
                                end;
                             end;

                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2<8) then
                             begin
                                while (i_2+1<=8)and(j_2+1<=8) and (WhotisPoledetector(i_2+1,j_2+1)<>cwhite) do
                                begin
                                   inc(i_2);
                                   inc(j_2);
                                   white_attack[i_2,j_2]:=true;
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2>1) then
                             begin
                                while (i_2-1>=1)and(j_2-1>=1) and (WhotisPoledetector(i_2-1,j_2-1)<>cwhite) do
                                begin
                                   dec(i_2);
                                   dec(j_2);
                                   white_attack[i_2,j_2]:=true;
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2<8) then
                             begin
                                while (i_2-1>=1) and (j_2+1<=8) and (WhotisPoledetector(i_2-1,j_2+1)<>cwhite) do
                                begin
                                   dec(i_2);
                                   inc(j_2);
                                   white_attack[i_2,j_2]:=true;
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2>1) then
                             begin
                                while (i_2+1<=8)and(j_2-1>=1) and (WhotisPoledetector(i_2+1,j_2-1)<>cwhite) do
                                begin
                                   inc(i_2);
                                   dec(j_2);
                                   white_attack[i_2,j_2]:=true;
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then break;
                                end;
                             end;
                          end;

                          if (arrwdetector[i_1].fig=cking) then
                          begin
                             // Ходы Короля
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2<8) then
                             begin
                                if (i_2+1<=8)and(j_2+1<=8) and (WhotisPoledetector(i_2+1,j_2+1)<>cwhite) then
                                begin
                                   inc(i_2);
                                   inc(j_2);
                                   white_attack[i_2,j_2]:=true;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2<8) then
                             begin
                                if (i_2-1>=1)and(j_2+1<=8) and (WhotisPoledetector(i_2-1,j_2+1)<>cwhite) then
                                begin
                                   dec(i_2);
                                   inc(j_2);
                                   white_attack[i_2,j_2]:=true;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (j_2<8) then
                             begin
                                if (j_2+1<=8) and (WhotisPoledetector(i_2,j_2+1)<>cwhite) then
                                begin
                                   inc(j_2);
                                   white_attack[i_2,j_2]:=true;
                                end;
                             end;

                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2>1) then
                             begin
                                if (i_2+1<=8)and(j_2-1>=1) and (WhotisPoledetector(i_2+1,j_2-1)<>cwhite) then
                                begin
                                   inc(i_2);
                                   dec(j_2);
                                   white_attack[i_2,j_2]:=true;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2>1) then
                             begin
                                if (i_2-1>=1)and(j_2-1>=1) and (WhotisPoledetector(i_2-1,j_2-1)<>cwhite) then
                                begin
                                   dec(i_2);
                                   dec(j_2);
                                   white_attack[i_2,j_2]:=true;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (j_2>1) then
                             begin
                                if (j_2-1>=1) and (WhotisPoledetector(i_2,j_2-1)<>cwhite) then
                                begin
                                   dec(j_2);
                                   white_attack[i_2,j_2]:=true;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8) then
                             begin
                                if (i_2+1<=8)and (WhotisPoledetector(i_2+1,j_2)<>cwhite) then
                                begin
                                   inc(i_2);
                                   white_attack[i_2,j_2]:=true;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1) then
                             begin
                                if (i_2-1>=1)and (WhotisPoledetector(i_2-1,j_2)<>cwhite) then
                                begin
                                   dec(i_2);
                                   white_attack[i_2,j_2]:=true;
                                end;
                             end;

                             // Рокировка в короткую сторону
                            { if ((WhotisPole(8,6)=cempty)and
                             (WhotisPole(8,7)=cempty)and
                             (WhotisPole(8,8)=cwhite)and
                             (WhotisPole(8,5)=cwhite)and
                             (WhotisFig(8,8)=crook)and
                             (WhotisFig(8,5)=cking)) then
                             begin
                                 i_2:=i;
                                 j_2:=j;
                                 if (j_2+2<=8) then
                                 begin
                                    vacantmove[i_2,j_2+2]:=true;
                                 end;
                             end;

                             // Рокировка в длинную сторону
                             if ((WhotisPole(8,2)=cempty)and
                             (WhotisPole(8,3)=cempty)and
                             (WhotisPole(8,4)=cempty)and
                             (WhotisPole(8,1)=cwhite)and
                             (WhotisPole(8,5)=cwhite)and
                             (WhotisFig(8,1)=crook)and
                             (WhotisFig(8,5)=cking)) then
                             begin
                                 i_2:=i;
                                 j_2:=j;
                                 if (j_2-2>=1) then
                                 begin
                                    vacantmove[i_2,j_2-2]:=true;
                                 end;
                             end;}

                          end;


      end;
   end;
end;

// Использует копию доски - detector.
procedure TForm1.GenerateBlackAttack(); // Генерирует поля которые бьют чёрные фигуры.
var
  i, j, i_1, i_3, i_2, j_2 : Integer;
begin
    for i:=1 to 8 do
      for j:=1 to 8 do
        black_attack[i,j]:=false;

    for i_1:=0 to High(arrbdetector) do
   begin
      if (arrbdetector[i_1].fig<>cemptyfig) then
      begin


      i:=arrbdetector[i_1].i;
      j:=arrbdetector[i_1].j;

       if (arrbdetector[i_1].fig=cpawn) then
                          begin

                              // пешка ест наискосок.
                             if ((j>1)and(WhotisPoledetector(i+1,j-1)<>cblack)) then  black_attack[i+1][j-1]:=true;
                             if ((j<8)and(WhotisPoledetector(i+1,j+1)<>cblack)) then  black_attack[i+1][j+1]:=true;

                             // Взятие на проходе
                             {if ((white_previos_move.fig=cpawn)and
                             (white_previos_move.from.i=7) and
                             (white_previos_move.to_.i=5) and
                             (((j>1)and(white_previos_move.from.j=j-1))or
                             ((j<8)and(white_previos_move.from.j=j+1))) and
                             (i=5)) then
                             begin
                                if ((j>1)and(white_previos_move.from.j=j-1)) then
                                begin
                                   black_attack[i+1][j-1]:=true;
                                end
                                else
                                begin
                                   black_attack[i+1][j+1]:=true;
                                end;
                             end;}
                          end;
                          if (arrbdetector[i_1].fig=cknight) then
                          begin
                             if (i+2<=8)and(j+1<=8)and(WhotisPoledetector(i+2,j+1)<>cblack) then black_attack[i+2][j+1]:=true;
                             if (i+1<=8)and(j+2<=8)and(WhotisPoledetector(i+1,j+2)<>cblack) then black_attack[i+1][j+2]:=true;
                             if (i-2>=1)and(j-1>=1)and(WhotisPoledetector(i-2,j-1)<>cblack) then black_attack[i-2][j-1]:=true;
                             if (i-1>=1)and(j-2>=1)and(WhotisPoledetector(i-1,j-2)<>cblack) then black_attack[i-1][j-2]:=true;
                             if (i+2<=8)and(j-1>=1)and(WhotisPoledetector(i+2,j-1)<>cblack) then black_attack[i+2][j-1]:=true;
                             if (i+1<=8)and(j-2>=1)and(WhotisPoledetector(i+1,j-2)<>cblack) then black_attack[i+1][j-2]:=true;
                             if (i-2>=1)and(j+1<=8)and(WhotisPoledetector(i-2,j+1)<>cblack) then black_attack[i-2][j+1]:=true;
                             if (i-1>=1)and(j+2<=8)and(WhotisPoledetector(i-1,j+2)<>cblack) then black_attack[i-1][j+2]:=true;
                          end;
                          if (arrbdetector[i_1].fig=crook) then
                          begin
                             i_3:=j;
                             if (i_3<8) then
                             begin
                                while (i_3+1<=8) and (WhotisPoledetector(i,i_3+1)<>cblack) do
                                begin
                                   inc(i_3);
                                   black_attack[i,i_3]:=true;
                                   if (WhotisPoledetector(i,i_3)=cwhite) then break;
                                end;
                             end;
                              i_3:=j;
                             if (i_3>1) then
                             begin
                                while (i_3-1>=1) and (WhotisPoledetector(i,i_3-1)<>cblack) do
                                begin
                                   dec(i_3);
                                   black_attack[i,i_3]:=true;
                                   if (WhotisPoledetector(i,i_3)=cwhite) then break;
                                end;
                             end;
                             i_3:=i;
                             if (i_3<8) then
                             begin
                                while (i_3+1<=8) and (WhotisPoledetector(i_3+1,j)<>cblack) do
                                begin
                                   inc(i_3);
                                   black_attack[i_3,j]:=true;
                                   if (WhotisPoledetector(i_3,j)=cwhite) then break;
                                end;
                             end;
                               i_3:=i;
                             if (i_3>1) then
                             begin
                                while (i_3-1>=1) and (WhotisPoledetector(i_3-1,j)<>cblack) do
                                begin
                                   dec(i_3);
                                   black_attack[i_3,j]:=true;
                                   if (WhotisPoledetector(i_3,j)=cwhite) then break;
                                end;
                             end;
                          end;
                           if (arrbdetector[i_1].fig=cbishop) then
                          begin
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2<8) then
                             begin
                                while (i_2+1<=8)and(j_2+1<=8) and (WhotisPoledetector(i_2+1,j_2+1)<>cblack) do
                                begin
                                   inc(i_2);
                                   inc(j_2);
                                   black_attack[i_2,j_2]:=true;
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2>1) then
                             begin
                                while (i_2-1>=1)and(j_2-1>=1) and (WhotisPoledetector(i_2-1,j_2-1)<>cblack) do
                                begin
                                   dec(i_2);
                                   dec(j_2);
                                   black_attack[i_2,j_2]:=true;
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2<8) then
                             begin
                                while (i_2-1>=1) and (j_2+1<=8) and (WhotisPoledetector(i_2-1,j_2+1)<>cblack) do
                                begin
                                   dec(i_2);
                                   inc(j_2);
                                   black_attack[i_2,j_2]:=true;
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2>1) then
                             begin
                                while (i_2+1<=8)and(j_2-1>=1) and (WhotisPoledetector(i_2+1,j_2-1)<>cblack) do
                                begin
                                   inc(i_2);
                                   dec(j_2);
                                   black_attack[i_2,j_2]:=true;
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then break;
                                end;
                             end;
                          end;
                          if (arrb[i_1].fig=cqueen) then
                          begin
                             // Королева - сочетание ладьи и слона.
                             i_3:=j;
                             if (i_3<8) then
                             begin
                                while (i_3+1<=8) and (WhotisPoledetector(i,i_3+1)<>cblack) do
                                begin
                                   // горизонталь в сторону увеличения
                                   inc(i_3);
                                   black_attack[i,i_3]:=true;
                                   if (WhotisPoledetector(i,i_3)=cwhite) then break;
                                end;
                             end;
                              i_3:=j;
                             if (i_3>1) then
                             begin
                                while (i_3-1>=1) and (WhotisPoledetector(i,i_3-1)<>cblack) do
                                begin
                                   dec(i_3);
                                   // горизонталь в сторону убывания
                                   black_attack[i,i_3]:=true;
                                   if (WhotisPoledetector(i,i_3)=cwhite) then break;
                                end;
                             end;
                             i_3:=i;
                             if (i_3<8) then
                             begin
                                while (i_3+1<=8) and (WhotisPoledetector(i_3+1,j)<>cblack) do
                                begin
                                   // Вертикаль вниз.
                                   inc(i_3);
                                   black_attack[i_3,j]:=true;
                                   if (WhotisPoledetector(i_3,j)=cwhite) then break;
                                end;
                             end;
                               i_3:=i;
                             if (i_3>1) then
                             begin
                                while (i_3-1>=1) and (WhotisPoledetector(i_3-1,j)<>cblack) do
                                begin
                                   // Вертикаль вверх
                                   dec(i_3);
                                   black_attack[i_3,j]:=true;
                                   if (WhotisPoledetector(i_3,j)=cwhite) then break;
                                end;
                             end;

                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2<8) then
                             begin
                                while (i_2+1<=8)and(j_2+1<=8) and (WhotisPoledetector(i_2+1,j_2+1)<>cblack) do
                                begin
                                   inc(i_2);
                                   inc(j_2);
                                   black_attack[i_2,j_2]:=true;
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2>1) then
                             begin
                                while (i_2-1>=1)and(j_2-1>=1) and (WhotisPoledetector(i_2-1,j_2-1)<>cblack) do
                                begin
                                   dec(i_2);
                                   dec(j_2);
                                   black_attack[i_2,j_2]:=true;
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2<8) then
                             begin
                                while (i_2-1>=1) and (j_2+1<=8) and (WhotisPoledetector(i_2-1,j_2+1)<>cblack) do
                                begin
                                   dec(i_2);
                                   inc(j_2);
                                   black_attack[i_2,j_2]:=true;
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2>1) then
                             begin
                                while (i_2+1<=8)and(j_2-1>=1) and (WhotisPoledetector(i_2+1,j_2-1)<>cblack) do
                                begin
                                   inc(i_2);
                                   dec(j_2);
                                   black_attack[i_2,j_2]:=true;
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then break;
                                end;
                             end;
                          end;

                          if (arrb[i_1].fig=cking) then
                          begin
                             // Ходы Короля
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2<8) then
                             begin
                                if (i_2+1<=8)and(j_2+1<=8) and (WhotisPoledetector(i_2+1,j_2+1)<>cblack) then
                                begin
                                   inc(i_2);
                                   inc(j_2);
                                   black_attack[i_2,j_2]:=true;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2<8) then
                             begin
                                if (i_2-1>=1)and(j_2+1<=8) and (WhotisPoledetector(i_2-1,j_2+1)<>cblack) then
                                begin
                                   dec(i_2);
                                   inc(j_2);
                                   black_attack[i_2,j_2]:=true;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (j_2<8) then
                             begin
                                if (j_2+1<=8) and (WhotisPoledetector(i_2,j_2+1)<>cblack) then
                                begin
                                   inc(j_2);
                                   black_attack[i_2,j_2]:=true;
                                end;
                             end;

                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2>1) then
                             begin
                                if (i_2+1<=8)and(j_2-1>=1) and (WhotisPoledetector(i_2+1,j_2-1)<>cblack) then
                                begin
                                   inc(i_2);
                                   dec(j_2);
                                   black_attack[i_2,j_2]:=true;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2>1) then
                             begin
                                if (i_2-1>=1)and(j_2-1>=1) and (WhotisPoledetector(i_2-1,j_2-1)<>cblack) then
                                begin
                                   dec(i_2);
                                   dec(j_2);
                                   black_attack[i_2,j_2]:=true;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (j_2>1) then
                             begin
                                if (j_2-1>=1) and (WhotisPoledetector(i_2,j_2-1)<>cblack) then
                                begin
                                   dec(j_2);
                                   black_attack[i_2,j_2]:=true;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8) then
                             begin
                                if (i_2+1<=8)and (WhotisPoledetector(i_2+1,j_2)<>cblack) then
                                begin
                                   inc(i_2);
                                   black_attack[i_2,j_2]:=true;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1) then
                             begin
                                if (i_2-1>=1)and (WhotisPoledetector(i_2-1,j_2)<>cblack) then
                                begin
                                   dec(i_2);
                                   black_attack[i_2,j_2]:=true;
                                end;
                             end;

                             // Рокировка в короткую сторону
                             {if ((WhotisPole(1,6)=cempty)and
                             (WhotisPole(1,7)=cempty)and
                             (WhotisPole(1,8)=cblack)and
                             (WhotisPole(1,5)=cblack)and
                             (WhotisFig(1,8)=crook)and
                             (WhotisFig(1,5)=cking)) then
                             begin
                                 i_2:=i;
                                 j_2:=j;
                                 if (j_2+2<=8) then
                                 begin
                                    vacantmove[i_2,j_2+2]:=true;
                                 end;
                             end;

                             // Рокировка в длинную сторону
                             if ((WhotisPole(1,2)=cempty)and
                             (WhotisPole(1,3)=cempty)and
                             (WhotisPole(1,4)=cempty)and
                             (WhotisPole(1,1)=cblack)and
                             (WhotisPole(1,5)=cblack)and
                             (WhotisFig(1,1)=crook)and
                             (WhotisFig(1,5)=cking)) then
                             begin
                                 i_2:=i;
                                 j_2:=j;
                                 if (j_2-2>=1) then
                                 begin
                                    vacantmove[i_2,j_2-2]:=true;
                                 end;
                             end;}
                          end;
        end;
      end;
end;

procedure TForm1.FormCreate(Sender: TObject);
var
  i,j : Integer;
begin
clientwidth:=540;
clientheight:=560;
position:=poScreenCenter;
bPress:=false;

    State:=0;

    // Фигуру которую нужно вернуть при операции UNDO.
    white_eating.fig:=cemptyfig;
    black_eating.fig:=cemptyfig;
    white_eating.i:=-1;
    white_eating.j:=-1;
    black_eating.i:=-1;
    black_eating.j:=-1;

    did_the_white_right_rook_move:=false;
    did_the_white_left_rook_move :=false; // ходили ли белые ладьи
    did_the_black_right_rook_move:=false;
    did_the_black_left_rook_move :=false; // ходили ли чёрные ладьи
    did_the_white_king_move:=false;
    did_the_black_king_move:=false; // ходили ли короли.

     did_the_white_right_rook_move1:=false;
    did_the_white_left_rook_move1 :=false; // ходили ли белые ладьи
    did_the_black_right_rook_move1:=false;
    did_the_black_left_rook_move1 :=false; // ходили ли чёрные ладьи
    did_the_white_king_move1:=false;
    did_the_black_king_move1:=false; // ходили ли короли.

white_previos_move.from.i:=-1;
white_previos_move.from.j:=-1;
white_previos_move.to_.i:=-1;
white_previos_move.to_.j:=-1;
white_previos_move.fig:=cemptyfig;

black_previos_move.from.i:=-1;
black_previos_move.from.j:=-1;
black_previos_move.to_.i:=-1;
black_previos_move.to_.j:=-1;
black_previos_move.fig:=cemptyfig;

white_previos_move1:=white_previos_move;
black_previos_move1:=black_previos_move;

for i:=1 to 8 do
for j:=1 to 8 do
vacantmove[i][j]:=false;
for i:=1 to 8 do
for j:=1 to 8 do
vacantmove1[i][j]:=false;

cMove:=cwhite; //  Белые начинают.

// Начальная расстановка войск.

SetLength(arrw,16);
SetLength(arrb,16);

arrw[0].fig:=cpawn;
arrw[0].i:=7;
arrw[0].j:=1;
arrw[1].fig:=cpawn;
arrw[1].i:=7;
arrw[1].j:=2;
arrw[2].fig:=cpawn;
arrw[2].i:=7;
arrw[2].j:=3;
arrw[3].fig:=cpawn;
arrw[3].i:=7;
arrw[3].j:=4;
arrw[4].fig:=cpawn;
arrw[4].i:=7;
arrw[4].j:=5;
arrw[5].fig:=cpawn;
arrw[5].i:=7;
arrw[5].j:=6;
arrw[6].fig:=cpawn;
arrw[6].i:=7;
arrw[6].j:=7;
arrw[7].fig:=cpawn;
arrw[7].i:=7;
arrw[7].j:=8;
arrw[8].fig:=crook;
arrw[8].i:=8;
arrw[8].j:=1;
arrw[9].fig:=crook;
arrw[9].i:=8;
arrw[9].j:=8;
arrw[10].fig:=cknight;
arrw[10].i:=8;
arrw[10].j:=2;
arrw[11].fig:=cknight;
arrw[11].i:=8;
arrw[11].j:=7;
arrw[12].fig:=cbishop;
arrw[12].i:=8;
arrw[12].j:=3;
arrw[13].fig:=cbishop;
arrw[13].i:=8;
arrw[13].j:=6;
arrw[14].fig:=cqueen;
arrw[14].i:=8;
arrw[14].j:=4;
arrw[15].fig:=cking;
arrw[15].i:=8;
arrw[15].j:=5;


arrb[0].fig:=cpawn;
arrb[0].i:=2;
arrb[0].j:=1;
arrb[1].fig:=cpawn;
arrb[1].i:=2;
arrb[1].j:=2;
arrb[2].fig:=cpawn;
arrb[2].i:=2;
arrb[2].j:=3;
arrb[3].fig:=cpawn;
arrb[3].i:=2;
arrb[3].j:=4;
arrb[4].fig:=cpawn;
arrb[4].i:=2;
arrb[4].j:=5;
arrb[5].fig:=cpawn;
arrb[5].i:=2;
arrb[5].j:=6;
arrb[6].fig:=cpawn;
arrb[6].i:=2;
arrb[6].j:=7;
arrb[7].fig:=cpawn;
arrb[7].i:=2;
arrb[7].j:=8;
arrb[8].fig:=crook;
arrb[8].i:=1;
arrb[8].j:=1;
arrb[9].fig:=crook;
arrb[9].i:=1;
arrb[9].j:=8;
arrb[10].fig:=cknight;
arrb[10].i:=1;
arrb[10].j:=2;
arrb[11].fig:=cknight;
arrb[11].i:=1;
arrb[11].j:=7;
arrb[12].fig:=cbishop;
arrb[12].i:=1;
arrb[12].j:=3;
arrb[13].fig:=cbishop;
arrb[13].i:=1;
arrb[13].j:=6;
arrb[14].fig:=cqueen;
arrb[14].i:=1;
arrb[14].j:=4;
arrb[15].fig:=cking;
arrb[15].i:=1;
arrb[15].j:=5;

SetLength(arrw1,16);
SetLength(arrb1,16);

for i:=0 to 15 do
begin
   arrw1[i]:=arrw[i];
   arrb1[i]:=arrb[i];
end;

end;

// if (cMove=cwhite) соблюдение очередности ходов.
procedure TForm1.FormMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
const n=8;
var i,j, i_1, i_2, j_2, i_3, i_4 : Integer;
    bO_O : Boolean; // признак рокировки.

begin
    bO_O:=false;

    if (bPress) then
    begin

       if (MatorPat(Sender)) then
       begin
          // Некуда идти мат или пат.
          // Game Over
          ShowMessage('Game over.');
          if (cMove=cwhite) then
                 begin
                     CopyList();
                     GenerateBlackAttack();
                     if (not(WhiteKing_Check(Sender))) then
                     begin
                        ShowMessage('Ничья. Пат.');
                     end
                     else
                     begin
                        // Белый король под шахом и ему некуда идти.
                        ShowMessage('Победа чёрных!.');
                     end;
                 end;
            if (cMove=cblack) then
                 begin
                     CopyList();
                     GenerateWhiteAttack();
                     if (not(BlackKing_Check(Sender))) then
                     begin
                        ShowMessage('Ничья. Пат.');
                     end
                     else
                     begin
                        // Чёрный король под шахом и ему некуда идти.
                        ShowMessage('Победа белых!.');
                     end;
                 end;
       end
       else
       begin

       for i:=1 to n do
          for j:=1 to n do
          begin
              if (bPress and ((xmouse>40+(j-1)*60)and(xmouse<40+(j-1)*60+60)and(
                   ymouse>20+(i-1)*60)and(ymouse<20+(i-1)*60+60))) then
              begin
                 // откуда
                 if (cMove=cwhite) then
                 begin
                    for i_1:=0 to High(arrw) do
                    begin
                       if ((arrw[i_1].fig<>cemptyfig) and (arrw[i_1].i=i) and (arrw[i_1].j=j)) then
                       begin
                          for i_2:=1 to n do
                            for j_2:=1 to n do
                            begin
                               if (bPress and ((X>40+(j_2-1)*60)and(X<40+(j_2-1)*60+60)and(
                                    Y>20+(i_2-1)*60)and(Y<20+(i_2-1)*60+60))) then
                               begin
                                  // куда
                                  if ((WhotisPole(i_2,j_2)<>cwhite)and(vacantmove[i_2,j_2])) then
                                  begin

                                     dec(State);
                                     if (State<0) then State:=0;

                                     // Резервное копирование состояния шахматной доски.
                                     SetLength(arrw1,Length(arrw));

                                     for i_4:=0 to High(arrw) do
                                     begin
                                        arrw1[i_4]:=arrw[i_4];
                                     end;

                                     white_previos_move1:=white_previos_move;


                                     // Правила должны соблюдаться нельзя белыми сьесть белых.
                                     // Правила для рокировки должны соблюдаться.

                                     if (arrw[i_1].fig=cking) and (abs(j_2-j)=2) then bO_O:=true;

                                     if (WhotisPole(i_2,j_2)=cblack) then
                                     begin
                                          black_eating.fig:=WhotisFig(i_2,j_2);
                                          black_eating.i:=i_2;
                                          black_eating.j:=j_2;
                                          DeleteFig(i_2,j_2); // Удаляем сьеденную вражескую фигуру.
                                     end
                                     else
                                     begin
                                        black_eating.fig:=cemptyfig;
                                        black_eating.i:=-1;
                                        black_eating.j:=-1;
                                     end;

                                     white_previos_move.from.i:=arrw[i_1].i;
                                     white_previos_move.from.j:=arrw[i_1].j;
                                     white_previos_move.to_.i:=i_2;
                                     white_previos_move.to_.j:=j_2;

                                     // запоминаем состояние.
                                     did_the_white_right_rook_move1:=did_the_white_right_rook_move;
                                     did_the_white_left_rook_move1 :=did_the_white_left_rook_move; // ходили ли белые ладьи
                                     did_the_white_king_move1:=did_the_white_king_move;

                                     if (arrw[i_1].fig=crook) and (arrw[i_1].i=8)and(arrw[i_1].j=8) then
                                     did_the_white_right_rook_move:=true;
                                     if (arrw[i_1].fig=crook) and (arrw[i_1].i=8)and(arrw[i_1].j=1) then
                                     did_the_white_left_rook_move:=true;
                                     if (arrw[i_1].fig=cking) and (arrw[i_1].i=8)and(arrw[i_1].j=5) then
                                     did_the_white_king_move:=true;


                                     // Удаление пешки при взятии на проходе
                                     if (WhotisPole(i_2,j_2)=cempty)and (arrw[i_1].fig=cpawn) and
                                     (j_2<>arrw[i_1].j) then
                                     begin
                                        black_eating.fig:=cpawn;
                                        black_eating.i:=arrw[i_1].i;
                                        black_eating.j:=j_2;
                                        DeleteFig(arrw[i_1].i,j_2); // Удаляем пешку при взятии на проходе.
                                     end;

                                     arrw[i_1].i:=i_2;
                                     arrw[i_1].j:=j_2;
                                     // Превращение пешки в королеву.
                                     if (i_2=1) and (arrw[i_1].fig=cpawn) then arrw[i_1].fig:=cqueen;
                                     white_previos_move.fig:=arrw[i_1].fig;

                                     if ((bO_O) and (j_2>5)) then
                                     begin
                                        // Рокировка в короткую сторону.
                                        // Двигаем Ладью.
                                         for i_3:=0 to High(arrw) do
                                        begin
                                           if ((arrw[i_3].i=8) and (arrw[i_3].j=8)and(arrw[i_3].fig=crook)) then
                                           begin
                                              arrw[i_3].j:=6; // переместили правую ладью.
                                           end;
                                        end;
                                     end;
                                     if ((bO_O) and (j_2<5)) then
                                     begin
                                        // Рокировка в длинную сторону.
                                        // Двигаем Ладью.
                                         for i_3:=0 to High(arrw) do
                                        begin
                                           if ((arrw[i_3].i=8) and (arrw[i_3].j=1)and(arrw[i_3].fig=crook)) then
                                           begin
                                              arrw[i_3].j:=4; // переместили левую ладью.
                                           end;
                                        end;
                                     end;

                                     cMove:=cblack;  // Передаём очередь хода чёрным.
                                  end;
                               end;
                            end;
                        end;
                     end;
                 end
                 else if (cMove=cblack) then
                 begin
                    for i_1:=0 to High(arrb) do
                    begin
                       if ((arrb[i_1].fig<>cemptyfig) and (arrb[i_1].i=i) and (arrb[i_1].j=j)) then
                       begin
                          for i_2:=1 to n do
                             for j_2:=1 to n do
                             begin
                                if (bPress and ((X>40+(j_2-1)*60)and(X<40+(j_2-1)*60+60)and(
                                     Y>20+(i_2-1)*60)and(Y<20+(i_2-1)*60+60))) then
                                begin
                                   // куда
                                   if ((WhotisPole(i_2,j_2)<>cblack)and(vacantmove[i_2,j_2])) then
                                   begin

                                        dec(State);
                                     if (State<0) then State:=0;


                                      // Резервное копирование состояния шахматной доски.
                                      SetLength(arrb1,Length(arrb));

                                      for i_4:=0 to High(arrb) do
                                      begin
                                         arrb1[i_4]:=arrb[i_4];
                                      end;

                                      black_previos_move1:=black_previos_move;

                                      // Правила должны соблюдаться нельзя чёрными сьесть чёрных.
                                      // Правила для рокировки должны соблюдаться.

                                      if (arrb[i_1].fig=cking) and (abs(j_2-j)=2) then bO_O:=true;
                                       if (WhotisPole(i_2,j_2)=cwhite) then
                                     begin
                                          white_eating.fig:=WhotisFig(i_2,j_2);
                                          white_eating.i:=i_2;
                                          white_eating.j:=j_2;
                                          DeleteFig(i_2,j_2); // Удаляем сьеденную вражескую фигуру.
                                     end
                                     else
                                     begin
                                        white_eating.fig:=cemptyfig;
                                        white_eating.i:=-1;
                                        white_eating.j:=-1;
                                     end;



                                      black_previos_move.from.i:=arrb[i_1].i;
                                      black_previos_move.from.j:=arrb[i_1].j;
                                      black_previos_move.to_.i:=i_2;
                                      black_previos_move.to_.j:=j_2;

                                      // запоминаем состояние.
                                     did_the_black_right_rook_move1:=did_the_black_right_rook_move;
                                     did_the_black_left_rook_move1 :=did_the_black_left_rook_move; // ходили ли белые ладьи
                                     did_the_black_king_move1:=did_the_black_king_move;

                                      if (arrb[i_1].fig=crook) and (arrb[i_1].i=1)and(arrb[i_1].j=8) then
                                     did_the_black_right_rook_move:=true;
                                     if (arrb[i_1].fig=crook) and (arrb[i_1].i=1)and(arrb[i_1].j=1) then
                                     did_the_black_left_rook_move:=true;
                                     if (arrb[i_1].fig=cking) and (arrb[i_1].i=1)and(arrb[i_1].j=5) then
                                     did_the_black_king_move:=true;

                                     // Удаление пешки при взятии на проходе
                                     if (WhotisPole(i_2,j_2)=cempty)and (arrb[i_1].fig=cpawn) and
                                     (j_2<>arrb[i_1].j) then
                                     begin
                                        white_eating.fig:=cpawn;
                                        white_eating.i:=arrb[i_1].i;
                                        white_eating.j:=j_2;
                                        DeleteFig(arrb[i_1].i,j_2); // Удаляем пешку при взятии на проходе.
                                     end;

                                      arrb[i_1].i:=i_2;
                                      arrb[i_1].j:=j_2;
                                       // Превращение пешки в королеву.
                                     if (i_2=8) and (arrb[i_1].fig=cpawn) then arrb[i_1].fig:=cqueen;
                                     black_previos_move.fig:=arrb[i_1].fig;

                                       if ((bO_O) and (j_2>5)) then
                                     begin
                                        // Рокировка в короткую сторону.
                                        // Двигаем Ладью.
                                         for i_3:=0 to High(arrb) do
                                        begin
                                           if ((arrb[i_3].i=1) and (arrb[i_3].j=8)and(arrb[i_3].fig=crook)) then
                                           begin
                                              arrb[i_3].j:=6; // переместили правую ладью.
                                           end;
                                        end;
                                     end;
                                     if ((bO_O) and (j_2<5)) then
                                     begin
                                        // Рокировка в длинную сторону.
                                        // Двигаем Ладью.
                                         for i_3:=0 to High(arrb) do
                                        begin
                                           if ((arrb[i_3].i=1) and (arrb[i_3].j=1)and(arrb[i_3].fig=crook)) then
                                           begin
                                              arrb[i_3].j:=4; // переместили левую ладью.
                                           end;
                                        end;
                                     end;

                                      cMove:=cwhite;  // Передаём очередь хода белым.

                                   end;
                                end;
                             end;
                         end;
                      end;
                 end;
              end;
          end;

       end;
    end;

    xmouse:=X;
    ymouse:=Y;
    bPress:=not(bPress);

    if (bPress) then
    begin
       for i:=1 to 8 do
       for j:=1 to 8 do
           vacantmove[i][j]:=false;

       for i:=1 to 8 do
       for j:=1 to 8 do
           vacantmove1[i][j]:=false;

       for i:=1 to n do
          for j:=1 to n do
          begin
              if (bPress and ((xmouse>40+(j-1)*60)and(xmouse<40+(j-1)*60+60)and(
                   ymouse>20+(i-1)*60)and(ymouse<20+(i-1)*60+60))) then
              begin
                 // откуда
                 if (cMove=cwhite) then
                 begin
                    for i_1:=0 to High(arrw) do
                    begin
                       if ((arrw[i_1].fig<>cemptyfig) and(arrw[i_1].i=i) and (arrw[i_1].j=j)) then
                       begin
                          // 1. Сделать предлагаемый виртуальный ход за белых.
                          // 2. Проверить находится ли белый король после этого хода под шахом.
                          // 3. Если шаха нет то ход допустим.

                          if (arrw[i_1].fig=cpawn) then
                          begin
                             if (WhotisPole(i-1,j)=cempty) then
                             begin
                                 CopyList();
                                    if (WhotisPoledetector(i-1,j)=cblack) then DeleteFigdetector(i-1,j);  // скушали фигурку.
                                    arrwdetector[i_1].i:=i-1;
                                    arrwdetector[i_1].j:=j;
                                    GenerateBlackAttack();
                                    if (not(WhiteKing_Check(Sender))) then
                                    begin
                                       vacantmove[i-1][j]:=true;
                                    end;
                             end;
                             if (i=7) and (WhotisPole(i-2,j)=cempty) then
                             begin
                                CopyList();
                                    if (WhotisPoledetector(i-2,j)=cblack) then DeleteFigdetector(i-2,j);  // скушали фигурку.
                                    arrwdetector[i_1].i:=i-2;
                                    arrwdetector[i_1].j:=j;
                                    GenerateBlackAttack();
                                    if (not(WhiteKing_Check(Sender))) then
                                    begin
                                        vacantmove[i-2][j]:=true;
                                    end;
                             end;
                             // пешка ест наискосок.
                             if ((j>1)and(WhotisPole(i-1,j-1)=cblack)) then
                             begin
                                 CopyList();
                                    if (WhotisPoledetector(i-1,j-1)=cblack) then DeleteFigdetector(i-1,j-1);  // скушали фигурку.
                                    arrwdetector[i_1].i:=i-1;
                                    arrwdetector[i_1].j:=j-1;
                                    GenerateBlackAttack();
                                    if (not(WhiteKing_Check(Sender))) then
                                    begin
                                       vacantmove[i-1][j-1]:=true;
                                   end;
                             end;
                             if ((j<8)and(WhotisPole(i-1,j+1)=cblack)) then
                             begin
                                CopyList();
                                    if (WhotisPoledetector(i-1,j+1)=cblack) then DeleteFigdetector(i-1,j+1);  // скушали фигурку.
                                    arrwdetector[i_1].i:=i-1;
                                    arrwdetector[i_1].j:=j+1;
                                    GenerateBlackAttack();
                                    if (not(WhiteKing_Check(Sender))) then
                                    begin
                                       vacantmove[i-1][j+1]:=true;
                                    end;
                             end;

                             // Взятие на проходе
                             if ((black_previos_move.fig=cpawn)and
                             (black_previos_move.from.i=2) and
                             (black_previos_move.to_.i=4) and
                             (((j>1)and(black_previos_move.from.j=j-1))or
                             ((j<8)and(black_previos_move.from.j=j+1))) and
                             (i=4)) then
                             begin
                                if ((j>1)and(black_previos_move.from.j=j-1)) then
                                begin
                                 CopyList();
                                    if (WhotisPoledetector(i-1,j-1)=cblack) then DeleteFigdetector(i-1,j-1);  // скушали фигурку.
                                    arrwdetector[i_1].i:=i-1;
                                    arrwdetector[i_1].j:=j-1;
                                    GenerateBlackAttack();
                                    if (not(WhiteKing_Check(Sender))) then
                                    begin
                                       vacantmove[i-1][j-1]:=true;
                                   end;
                                end
                                else
                                begin
                                   CopyList();
                                    if (WhotisPoledetector(i-1,j+1)=cblack) then DeleteFigdetector(i-1,j+1);  // скушали фигурку.
                                    arrwdetector[i_1].i:=i-1;
                                    arrwdetector[i_1].j:=j+1;
                                    GenerateBlackAttack();
                                    if (not(WhiteKing_Check(Sender))) then
                                    begin
                                       vacantmove[i-1][j+1]:=true;
                                   end;
                                end;
                             end;
                          end;
                          if (arrw[i_1].fig=cknight) then
                          begin
                             if (i+2<=8)and(j+1<=8)and(WhotisPole(i+2,j+1)<>cwhite) then
                             begin
                                 CopyList();
                                 if (WhotisPoledetector(i+2,j+1)=cblack) then DeleteFigdetector(i+2,j+1);  // скушали фигурку.
                                 arrwdetector[i_1].i:=i+2;
                                 arrwdetector[i_1].j:=j+1;
                                 GenerateBlackAttack();
                                 if (not(WhiteKing_Check(Sender))) then
                                 begin
                                    vacantmove[i+2][j+1]:=true;
                                 end;
                             end;
                             if (i+1<=8)and(j+2<=8)and(WhotisPole(i+1,j+2)<>cwhite) then
                             begin
                                CopyList();
                                 if (WhotisPoledetector(i+1,j+2)=cblack) then DeleteFigdetector(i+1,j+2);  // скушали фигурку.
                                 arrwdetector[i_1].i:=i+1;
                                 arrwdetector[i_1].j:=j+2;
                                 GenerateBlackAttack();
                                 if (not(WhiteKing_Check(Sender))) then
                                 begin
                                    vacantmove[i+1][j+2]:=true;
                                end;
                             end;
                             if (i-2>=1)and(j-1>=1)and(WhotisPole(i-2,j-1)<>cwhite) then
                             begin
                                 CopyList();
                                 if (WhotisPoledetector(i-2,j-1)=cblack) then
                                 begin
                                     DeleteFigdetector(i-2,j-1);  // скушали фигурку.
                                 end;
                                 arrwdetector[i_1].i:=i-2;
                                 arrwdetector[i_1].j:=j-1;
                                 GenerateBlackAttack();
                                 //DrawBlackAttack(Sender);
                                 if (not(WhiteKing_Check(Sender))) then
                                 begin
                                    vacantmove[i-2][j-1]:=true;
                                 end;
                             end;
                             if (i-1>=1)and(j-2>=1)and(WhotisPole(i-1,j-2)<>cwhite) then
                             begin
                                 CopyList();
                                  if (WhotisPoledetector(i-1,j-2)=cblack) then DeleteFigdetector(i-1,j-2);  // скушали фигурку.
                                 arrwdetector[i_1].i:=i-1;
                                 arrwdetector[i_1].j:=j-2;
                                 GenerateBlackAttack();
                                 if (not(WhiteKing_Check(Sender))) then
                                 begin
                                    vacantmove[i-1][j-2]:=true;
                                 end;
                             end;
                             if (i+2<=8)and(j-1>=1)and(WhotisPole(i+2,j-1)<>cwhite) then
                             begin
                                 CopyList();
                                  if (WhotisPoledetector(i+2,j-1)=cblack) then DeleteFigdetector(i+2,j-1);  // скушали фигурку.
                                 arrwdetector[i_1].i:=i+2;
                                 arrwdetector[i_1].j:=j-1;
                                 GenerateBlackAttack();
                                 if (not(WhiteKing_Check(Sender))) then
                                 begin
                                    vacantmove[i+2][j-1]:=true;
                                 end;
                             end;
                             if (i+1<=8)and(j-2>=1)and(WhotisPole(i+1,j-2)<>cwhite) then
                             begin
                                 CopyList();
                                  if (WhotisPoledetector(i+1,j-2)=cblack) then DeleteFigdetector(i+1,j-2);  // скушали фигурку.
                                 arrwdetector[i_1].i:=i+1;
                                 arrwdetector[i_1].j:=j-2;
                                 GenerateBlackAttack();
                                 if (not(WhiteKing_Check(Sender))) then
                                 begin
                                    vacantmove[i+1][j-2]:=true;
                                 end;
                             end;
                             if (i-2>=1)and(j+1<=8)and(WhotisPole(i-2,j+1)<>cwhite) then
                             begin
                                CopyList();
                                 if (WhotisPoledetector(i-2,j+1)=cblack) then DeleteFigdetector(i-2,j+1);  // скушали фигурку.
                                 arrwdetector[i_1].i:=i-2;
                                 arrwdetector[i_1].j:=j+1;
                                 GenerateBlackAttack();
                                 if (not(WhiteKing_Check(Sender))) then
                                 begin
                                    vacantmove[i-2][j+1]:=true;
                                 end;
                             end;
                             if (i-1>=1)and(j+2<=8)and(WhotisPole(i-1,j+2)<>cwhite) then
                             begin
                                 CopyList();
                                  if (WhotisPoledetector(i-1,j+2)=cblack) then DeleteFigdetector(i-1,j+2);  // скушали фигурку.
                                 arrwdetector[i_1].i:=i-1;
                                 arrwdetector[i_1].j:=j+2;
                                 GenerateBlackAttack();
                                 if (not(WhiteKing_Check(Sender))) then
                                 begin
                                    vacantmove[i-1][j+2]:=true;
                                 end;
                             end;
                          end;
                          if (arrw[i_1].fig=crook) then
                          begin
                             i_3:=j;
                             if (i_3<8) then
                             begin
                                while (i_3+1<=8) and (WhotisPole(i,i_3+1)<>cwhite) do
                                begin
                                   inc(i_3);
                                   CopyList();
                                   if (WhotisPoledetector(i,i_3)=cblack) then DeleteFigdetector(i,i_3);  // скушали фигурку.
                                   arrwdetector[i_1].i:=i;
                                   arrwdetector[i_1].j:=i_3;
                                   GenerateBlackAttack();
                                   if (not(WhiteKing_Check(Sender))) then
                                   begin
                                      vacantmove[i,i_3]:=true;
                                   end;
                                   if (WhotisPole(i,i_3)=cblack) then break;
                                end;
                             end;
                              i_3:=j;
                             if (i_3>1) then
                             begin
                                while (i_3-1>=1) and (WhotisPole(i,i_3-1)<>cwhite) do
                                begin
                                   dec(i_3);
                                    CopyList();
                                    if (WhotisPoledetector(i,i_3)=cblack) then DeleteFigdetector(i,i_3);  // скушали фигурку.
                                    arrwdetector[i_1].i:=i;
                                    arrwdetector[i_1].j:=i_3;
                                    GenerateBlackAttack();
                                    if (not(WhiteKing_Check(Sender))) then
                                    begin
                                       vacantmove[i,i_3]:=true;
                                    end;
                                   if (WhotisPole(i,i_3)=cblack) then break;
                                end;
                             end;
                             i_3:=i;
                             if (i_3<8) then
                             begin
                                while (i_3+1<=8) and (WhotisPole(i_3+1,j)<>cwhite) do
                                begin
                                   inc(i_3);
                                   CopyList();
                                    if (WhotisPoledetector(i_3,j)=cblack) then DeleteFigdetector(i_3,j);  // скушали фигурку.
                                    arrwdetector[i_1].i:=i_3;
                                    arrwdetector[i_1].j:=j;
                                    GenerateBlackAttack();
                                    if (not(WhiteKing_Check(Sender))) then
                                    begin
                                       vacantmove[i_3,j]:=true;
                                    end;
                                   if (WhotisPole(i_3,j)=cblack) then break;
                                end;
                             end;
                               i_3:=i;
                             if (i_3>1) then
                             begin
                                while (i_3-1>=1) and (WhotisPole(i_3-1,j)<>cwhite) do
                                begin
                                   dec(i_3);
                                   CopyList();
                                    if (WhotisPoledetector(i_3,j)=cblack) then DeleteFigdetector(i_3,j);  // скушали фигурку.
                                    arrwdetector[i_1].i:=i_3;
                                    arrwdetector[i_1].j:=j;
                                    GenerateBlackAttack();
                                    if (not(WhiteKing_Check(Sender))) then
                                    begin
                                       vacantmove[i_3,j]:=true;
                                    end;
                                   if (WhotisPole(i_3,j)=cblack) then break;
                                end;
                             end;
                          end;
                           if (arrw[i_1].fig=cbishop) then
                          begin
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2<8) then
                             begin
                                while (i_2+1<=8)and(j_2+1<=8) and (WhotisPole(i_2+1,j_2+1)<>cwhite) do
                                begin
                                   inc(i_2);
                                   inc(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrwdetector[i_1].i:=i_2;
                                   arrwdetector[i_1].j:=j_2;
                                   GenerateBlackAttack();
                                   if (not(WhiteKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                   if (WhotisPole(i_2,j_2)=cblack) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2>1) then
                             begin
                                while (i_2-1>=1)and(j_2-1>=1) and (WhotisPole(i_2-1,j_2-1)<>cwhite) do
                                begin
                                   dec(i_2);
                                   dec(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrwdetector[i_1].i:=i_2;
                                   arrwdetector[i_1].j:=j_2;
                                   GenerateBlackAttack();
                                   if (not(WhiteKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                   if (WhotisPole(i_2,j_2)=cblack) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2<8) then
                             begin
                                while (i_2-1>=1) and (j_2+1<=8) and (WhotisPole(i_2-1,j_2+1)<>cwhite) do
                                begin
                                   dec(i_2);
                                   inc(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrwdetector[i_1].i:=i_2;
                                   arrwdetector[i_1].j:=j_2;
                                   GenerateBlackAttack();
                                   if (not(WhiteKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                   if (WhotisPole(i_2,j_2)=cblack) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2>1) then
                             begin
                                while (i_2+1<=8)and(j_2-1>=1) and (WhotisPole(i_2+1,j_2-1)<>cwhite) do
                                begin
                                   inc(i_2);
                                   dec(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrwdetector[i_1].i:=i_2;
                                   arrwdetector[i_1].j:=j_2;
                                   GenerateBlackAttack();
                                   if (not(WhiteKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                   if (WhotisPole(i_2,j_2)=cblack) then break;
                                end;
                             end;
                          end;
                          if (arrw[i_1].fig=cqueen) then
                          begin
                             // Королева - сочетание ладьи и слона.
                             i_3:=j;
                             if (i_3<8) then
                             begin
                                while (i_3+1<=8) and (WhotisPole(i,i_3+1)<>cwhite) do
                                begin
                                   inc(i_3);
                                    CopyList();
                                   if (WhotisPoledetector(i,i_3)=cblack) then DeleteFigdetector(i,i_3);  // скушали фигурку.
                                   arrwdetector[i_1].i:=i;
                                   arrwdetector[i_1].j:=i_3;
                                   GenerateBlackAttack();
                                   if (not(WhiteKing_Check(Sender))) then
                                   begin
                                      vacantmove[i,i_3]:=true;
                                   end;
                                   if (WhotisPole(i,i_3)=cblack) then break;
                                end;
                             end;
                              i_3:=j;
                             if (i_3>1) then
                             begin
                                while (i_3-1>=1) and (WhotisPole(i,i_3-1)<>cwhite) do
                                begin
                                   dec(i_3);
                                    CopyList();
                                   if (WhotisPoledetector(i,i_3)=cblack) then DeleteFigdetector(i,i_3);  // скушали фигурку.
                                   arrwdetector[i_1].i:=i;
                                   arrwdetector[i_1].j:=i_3;
                                   GenerateBlackAttack();
                                   if (not(WhiteKing_Check(Sender))) then
                                   begin
                                      vacantmove[i,i_3]:=true;
                                   end;
                                   if (WhotisPole(i,i_3)=cblack) then break;
                                end;
                             end;
                             i_3:=i;
                             if (i_3<8) then
                             begin
                                while (i_3+1<=8) and (WhotisPole(i_3+1,j)<>cwhite) do
                                begin
                                   inc(i_3);
                                   CopyList();
                                    if (WhotisPoledetector(i_3,j)=cblack) then DeleteFigdetector(i_3,j);  // скушали фигурку.
                                    arrwdetector[i_1].i:=i_3;
                                    arrwdetector[i_1].j:=j;
                                    GenerateBlackAttack();
                                    if (not(WhiteKing_Check(Sender))) then
                                    begin
                                       vacantmove[i_3,j]:=true;
                                    end;
                                   if (WhotisPole(i_3,j)=cblack) then break;
                                end;
                             end;
                               i_3:=i;
                             if (i_3>1) then
                             begin
                                while (i_3-1>=1) and (WhotisPole(i_3-1,j)<>cwhite) do
                                begin
                                   dec(i_3);
                                    CopyList();
                                    if (WhotisPoledetector(i_3,j)=cblack) then DeleteFigdetector(i_3,j);  // скушали фигурку.
                                    arrwdetector[i_1].i:=i_3;
                                    arrwdetector[i_1].j:=j;
                                    GenerateBlackAttack();
                                    if (not(WhiteKing_Check(Sender))) then
                                    begin
                                       vacantmove[i_3,j]:=true;
                                    end;
                                   if (WhotisPole(i_3,j)=cblack) then break;
                                end;
                             end;

                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2<8) then
                             begin
                                while (i_2+1<=8)and(j_2+1<=8) and (WhotisPole(i_2+1,j_2+1)<>cwhite) do
                                begin
                                   inc(i_2);
                                   inc(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrwdetector[i_1].i:=i_2;
                                   arrwdetector[i_1].j:=j_2;
                                   GenerateBlackAttack();
                                   if (not(WhiteKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                   if (WhotisPole(i_2,j_2)=cblack) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2>1) then
                             begin
                                while (i_2-1>=1)and(j_2-1>=1) and (WhotisPole(i_2-1,j_2-1)<>cwhite) do
                                begin
                                   dec(i_2);
                                   dec(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrwdetector[i_1].i:=i_2;
                                   arrwdetector[i_1].j:=j_2;
                                   GenerateBlackAttack();
                                   if (not(WhiteKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                   if (WhotisPole(i_2,j_2)=cblack) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2<8) then
                             begin
                                while (i_2-1>=1) and (j_2+1<=8) and (WhotisPole(i_2-1,j_2+1)<>cwhite) do
                                begin
                                   dec(i_2);
                                   inc(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrwdetector[i_1].i:=i_2;
                                   arrwdetector[i_1].j:=j_2;
                                   GenerateBlackAttack();
                                   if (not(WhiteKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                   if (WhotisPole(i_2,j_2)=cblack) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2>1) then
                             begin
                                while (i_2+1<=8)and(j_2-1>=1) and (WhotisPole(i_2+1,j_2-1)<>cwhite) do
                                begin
                                   inc(i_2);
                                   dec(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrwdetector[i_1].i:=i_2;
                                   arrwdetector[i_1].j:=j_2;
                                   GenerateBlackAttack();
                                   if (not(WhiteKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                   if (WhotisPole(i_2,j_2)=cblack) then break;
                                end;
                             end;
                          end;

                          if (arrw[i_1].fig=cking) then
                          begin
                             // Ходы Короля
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2<8) then
                             begin
                                if (i_2+1<=8)and(j_2+1<=8) and (WhotisPole(i_2+1,j_2+1)<>cwhite) then
                                begin
                                   inc(i_2);
                                   inc(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrwdetector[i_1].i:=i_2;
                                   arrwdetector[i_1].j:=j_2;
                                   GenerateBlackAttack();
                                   if (not(WhiteKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2<8) then
                             begin
                                if (i_2-1>=1)and(j_2+1<=8) and (WhotisPole(i_2-1,j_2+1)<>cwhite) then
                                begin
                                   dec(i_2);
                                   inc(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrwdetector[i_1].i:=i_2;
                                   arrwdetector[i_1].j:=j_2;
                                   GenerateBlackAttack();
                                   if (not(WhiteKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (j_2<8) then
                             begin
                                if (j_2+1<=8) and (WhotisPole(i_2,j_2+1)<>cwhite) then
                                begin
                                   inc(j_2);
                                   CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrwdetector[i_1].i:=i_2;
                                   arrwdetector[i_1].j:=j_2;
                                   GenerateBlackAttack();
                                   if (not(WhiteKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                end;
                             end;

                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2>1) then
                             begin
                                if (i_2+1<=8)and(j_2-1>=1) and (WhotisPole(i_2+1,j_2-1)<>cwhite) then
                                begin
                                   inc(i_2);
                                   dec(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrwdetector[i_1].i:=i_2;
                                   arrwdetector[i_1].j:=j_2;
                                   GenerateBlackAttack();
                                   if (not(WhiteKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2>1) then
                             begin
                                if (i_2-1>=1)and(j_2-1>=1) and (WhotisPole(i_2-1,j_2-1)<>cwhite) then
                                begin
                                   dec(i_2);
                                   dec(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrwdetector[i_1].i:=i_2;
                                   arrwdetector[i_1].j:=j_2;
                                   GenerateBlackAttack();
                                   if (not(WhiteKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (j_2>1) then
                             begin
                                if (j_2-1>=1) and (WhotisPole(i_2,j_2-1)<>cwhite) then
                                begin
                                   dec(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrwdetector[i_1].i:=i_2;
                                   arrwdetector[i_1].j:=j_2;
                                   GenerateBlackAttack();
                                   if (not(WhiteKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8) then
                             begin
                                if (i_2+1<=8)and (WhotisPole(i_2+1,j_2)<>cwhite) then
                                begin
                                   inc(i_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrwdetector[i_1].i:=i_2;
                                   arrwdetector[i_1].j:=j_2;
                                   GenerateBlackAttack();
                                   if (not(WhiteKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1) then
                             begin
                                if (i_2-1>=1)and (WhotisPole(i_2-1,j_2)<>cwhite) then
                                begin
                                   dec(i_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cblack) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrwdetector[i_1].i:=i_2;
                                   arrwdetector[i_1].j:=j_2;
                                   GenerateBlackAttack();
                                   if (not(WhiteKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                end;
                             end;

                             // Рокировка в короткую сторону
                             if ((WhotisPole(8,6)=cempty)and
                             (WhotisPole(8,7)=cempty)and
                             (WhotisPole(8,8)=cwhite)and
                             (WhotisPole(8,5)=cwhite)and
                             (WhotisFig(8,8)=crook)and
                             (WhotisFig(8,5)=cking)) then
                             begin
                                 i_2:=i;
                                 j_2:=j;
                                 if (j_2+2<=8) then
                                 begin
                                    CopyList();
                                    GenerateBlackAttack();
                                    if ((black_attack[8,6])or(black_attack[8,7])or
                                    (black_attack[8,5])or did_the_white_right_rook_move
                                    or did_the_white_king_move) then
                                    begin
                                       // рокировка невозможна.
                                       // если поле, пройденное королём во время рокировки, находится под боем противника;
                                       //если король перед началом рокировки находится под шахом или после её осуществления попадает под шах;
                                       // ходила правая ладья или уже ходил король.
                                    end
                                    else
                                    begin
                                       vacantmove[i_2,j_2+2]:=true;
                                    end;
                                 end;
                             end;

                             // Рокировка в длинную сторону
                             if ((WhotisPole(8,2)=cempty)and
                             (WhotisPole(8,3)=cempty)and
                             (WhotisPole(8,4)=cempty)and
                             (WhotisPole(8,1)=cwhite)and
                             (WhotisPole(8,5)=cwhite)and
                             (WhotisFig(8,1)=crook)and
                             (WhotisFig(8,5)=cking)) then
                             begin
                                 i_2:=i;
                                 j_2:=j;
                                 if (j_2-2>=1) then
                                 begin
                                    CopyList();
                                    GenerateBlackAttack();
                                    if ((black_attack[8,3])or(black_attack[8,4])or
                                    (black_attack[8,5])or did_the_white_left_rook_move
                                    or did_the_white_king_move) then
                                    begin
                                       // рокировка невозможна.
                                       // если поле, пройденное королём во время рокировки, находится под боем противника;
                                       //если король перед началом рокировки находится под шахом или после её осуществления попадает под шах;
                                       // ходила левая ладья или уже ходил король.
                                    end
                                    else
                                    begin
                                       vacantmove[i_2,j_2-2]:=true;
                                    end;
                                 end;
                             end;

                          end;

                       end;
                    end;
                 end;

                 if (cMove=cblack) then
                 begin
                    for i_1:=0 to High(arrb) do
                    begin
                       if ((arrb[i_1].fig<>cemptyfig) and (arrb[i_1].i=i) and (arrb[i_1].j=j)) then
                       begin
                          if (arrb[i_1].fig=cpawn) then
                          begin
                             if (WhotisPole(i+1,j)=cempty) then
                             begin
                                CopyList();
                                   if (WhotisPoledetector(i+1,j)=cwhite) then DeleteFigdetector(i+1,j);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i+1;
                                   arrbdetector[i_1].j:=j;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                       vacantmove[i+1][j]:=true;
                                   end;
                             end;
                             if (i=2) and (WhotisPole(i+2,j)=cempty) then
                             begin
                                CopyList();
                                   if (WhotisPoledetector(i+2,j)=cwhite) then DeleteFigdetector(i+2,j);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i+2;
                                   arrbdetector[i_1].j:=j;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i+2][j]:=true;
                                   end;
                             end;
                              // пешка ест наискосок.
                             if ((j>1)and(WhotisPole(i+1,j-1)=cwhite)) then
                             begin
                                CopyList();
                                if (WhotisPoledetector(i+1,j-1)=cwhite) then DeleteFigdetector(i+1,j-1);  // скушали фигурку.
                                arrbdetector[i_1].i:=i+1;
                                arrbdetector[i_1].j:=j-1;
                                GenerateWhiteAttack();
                                if (not(BlackKing_Check(Sender))) then
                                begin
                                   vacantmove[i+1][j-1]:=true;
                                end;
                             end;
                             if ((j<8)and(WhotisPole(i+1,j+1)=cwhite)) then
                             begin
                                CopyList();
                                   if (WhotisPoledetector(i+1,j+1)=cwhite) then DeleteFigdetector(i+1,j+1);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i+1;
                                   arrbdetector[i_1].j:=j+1;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                   vacantmove[i+1][j+1]:=true;
                                end;
                             end;


                             // Взятие на проходе
                             if ((white_previos_move.fig=cpawn)and
                             (white_previos_move.from.i=7) and
                             (white_previos_move.to_.i=5) and
                             (((j>1)and(white_previos_move.from.j=j-1))or
                             ((j<8)and(white_previos_move.from.j=j+1))) and
                             (i=5)) then
                             begin
                                if ((j>1)and(white_previos_move.from.j=j-1)) then
                                begin
                                   CopyList();
                                   if (WhotisPoledetector(i+1,j-1)=cwhite) then DeleteFigdetector(i+1,j-1);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i+1;
                                   arrbdetector[i_1].j:=j-1;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i+1][j-1]:=true;
                                   end;
                                end
                                else
                                begin
                                   CopyList();
                                   if (WhotisPoledetector(i+1,j+1)=cwhite) then DeleteFigdetector(i+1,j+1);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i+1;
                                   arrbdetector[i_1].j:=j+1;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                       vacantmove[i+1][j+1]:=true;
                                   end;
                                end;
                             end;
                          end;
                          if (arrb[i_1].fig=cknight) then
                          begin
                             if (i+2<=8)and(j+1<=8)and(WhotisPole(i+2,j+1)<>cblack) then
                             begin
                                 CopyList();
                                   if (WhotisPoledetector(i+2,j+1)=cwhite) then DeleteFigdetector(i+2,j+1);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i+2;
                                   arrbdetector[i_1].j:=j+1;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i+2][j+1]:=true;
                                   end;
                             end;
                             if (i+1<=8)and(j+2<=8)and(WhotisPole(i+1,j+2)<>cblack) then
                             begin
                                CopyList();
                                   if (WhotisPoledetector(i+1,j+2)=cwhite) then DeleteFigdetector(i+1,j+2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i+1;
                                   arrbdetector[i_1].j:=j+2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i+1][j+2]:=true;
                                   end;
                             end;
                             if (i-2>=1)and(j-1>=1)and(WhotisPole(i-2,j-1)<>cblack) then
                             begin
                                CopyList();
                                   if (WhotisPoledetector(i-2,j-1)=cwhite) then DeleteFigdetector(i-2,j-1);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i-2;
                                   arrbdetector[i_1].j:=j-1;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i-2][j-1]:=true;
                                   end;
                             end;
                             if (i-1>=1)and(j-2>=1)and(WhotisPole(i-1,j-2)<>cblack) then
                             begin
                                   CopyList();
                                   if (WhotisPoledetector(i-1,j-2)=cwhite) then DeleteFigdetector(i-1,j-2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i-1;
                                   arrbdetector[i_1].j:=j-2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i-1][j-2]:=true;
                                   end;
                             end;
                             if (i+2<=8)and(j-1>=1)and(WhotisPole(i+2,j-1)<>cblack) then
                             begin
                                CopyList();
                                   if (WhotisPoledetector(i+2,j-1)=cwhite) then DeleteFigdetector(i+2,j-1);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i+2;
                                   arrbdetector[i_1].j:=j-1;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i+2][j-1]:=true;
                                   end;
                             end;
                             if (i+1<=8)and(j-2>=1)and(WhotisPole(i+1,j-2)<>cblack) then
                             begin
                                CopyList();
                                   if (WhotisPoledetector(i+1,j-2)=cwhite) then DeleteFigdetector(i+1,j-2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i+1;
                                   arrbdetector[i_1].j:=j-2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i+1][j-2]:=true;
                                   end;
                             end;
                             if (i-2>=1)and(j+1<=8)and(WhotisPole(i-2,j+1)<>cblack) then
                             begin
                                CopyList();
                                   if (WhotisPoledetector(i-2,j+1)=cwhite) then DeleteFigdetector(i-2,j+1);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i-2;
                                   arrbdetector[i_1].j:=j+1;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                       vacantmove[i-2][j+1]:=true;
                                   end;
                             end;
                             if (i-1>=1)and(j+2<=8)and(WhotisPole(i-1,j+2)<>cblack) then
                             begin
                                CopyList();
                                   if (WhotisPoledetector(i-1,j+2)=cwhite) then DeleteFigdetector(i-1,j+2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i-1;
                                   arrbdetector[i_1].j:=j+2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i-1][j+2]:=true;
                                   end;
                             end;
                          end;
                          if (arrb[i_1].fig=crook) then
                          begin
                             i_3:=j;
                             if (i_3<8) then
                             begin
                                while (i_3+1<=8) and (WhotisPole(i,i_3+1)<>cblack) do
                                begin
                                   inc(i_3);
                                    CopyList();
                                   if (WhotisPoledetector(i,i_3)=cwhite) then DeleteFigdetector(i,i_3);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i;
                                   arrbdetector[i_1].j:=i_3;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                   vacantmove[i,i_3]:=true;

                                   end;
                                   if (WhotisPole(i,i_3)=cwhite) then break;
                                end;
                             end;
                              i_3:=j;
                             if (i_3>1) then
                             begin
                                while (i_3-1>=1) and (WhotisPole(i,i_3-1)<>cblack) do
                                begin
                                   dec(i_3);
                                    CopyList();
                                   if (WhotisPoledetector(i,i_3)=cwhite) then DeleteFigdetector(i,i_3);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i;
                                   arrbdetector[i_1].j:=i_3;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                   vacantmove[i,i_3]:=true;

                                   end;
                                   if (WhotisPole(i,i_3)=cwhite) then break;
                                end;
                             end;
                             i_3:=i;
                             if (i_3<8) then
                             begin
                                while (i_3+1<=8) and (WhotisPole(i_3+1,j)<>cblack) do
                                begin
                                   inc(i_3);
                                    CopyList();
                                   if (WhotisPoledetector(i_3,j)=cwhite) then DeleteFigdetector(i_3,j);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_3;
                                   arrbdetector[i_1].j:=j;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_3,j]:=true;
                                   end;
                                   if (WhotisPole(i_3,j)=cwhite) then break;
                                end;
                             end;
                               i_3:=i;
                             if (i_3>1) then
                             begin
                                while (i_3-1>=1) and (WhotisPole(i_3-1,j)<>cblack) do
                                begin
                                   dec(i_3);
                                     CopyList();
                                   if (WhotisPoledetector(i_3,j)=cwhite) then DeleteFigdetector(i_3,j);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_3;
                                   arrbdetector[i_1].j:=j;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_3,j]:=true;
                                   end;
                                   if (WhotisPole(i_3,j)=cwhite) then break;
                                end;
                             end;
                          end;
                           if (arrb[i_1].fig=cbishop) then
                          begin
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2<8) then
                             begin
                                while (i_2+1<=8)and(j_2+1<=8) and (WhotisPole(i_2+1,j_2+1)<>cblack) do
                                begin
                                   inc(i_2);
                                   inc(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_2;
                                   arrbdetector[i_1].j:=j_2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                   if (WhotisPole(i_2,j_2)=cwhite) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2>1) then
                             begin
                                while (i_2-1>=1)and(j_2-1>=1) and (WhotisPole(i_2-1,j_2-1)<>cblack) do
                                begin
                                   dec(i_2);
                                   dec(j_2);
                                      CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_2;
                                   arrbdetector[i_1].j:=j_2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                   if (WhotisPole(i_2,j_2)=cwhite) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2<8) then
                             begin
                                while (i_2-1>=1) and (j_2+1<=8) and (WhotisPole(i_2-1,j_2+1)<>cblack) do
                                begin
                                   dec(i_2);
                                   inc(j_2);
                                      CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_2;
                                   arrbdetector[i_1].j:=j_2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                   if (WhotisPole(i_2,j_2)=cwhite) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2>1) then
                             begin
                                while (i_2+1<=8)and(j_2-1>=1) and (WhotisPole(i_2+1,j_2-1)<>cblack) do
                                begin
                                   inc(i_2);
                                   dec(j_2);
                                      CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_2;
                                   arrbdetector[i_1].j:=j_2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                   if (WhotisPole(i_2,j_2)=cwhite) then break;
                                end;
                             end;
                          end;
                          if (arrb[i_1].fig=cqueen) then
                          begin
                             // Королева - сочетание ладьи и слона.
                             i_3:=j;
                             if (i_3<8) then
                             begin
                                while (i_3+1<=8) and (WhotisPole(i,i_3+1)<>cblack) do
                                begin
                                   inc(i_3);
                                    CopyList();
                                   if (WhotisPoledetector(i,i_3)=cwhite) then DeleteFigdetector(i,i_3);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i;
                                   arrbdetector[i_1].j:=i_3;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                   vacantmove[i,i_3]:=true;

                                   end;
                                   if (WhotisPole(i,i_3)=cwhite) then break;
                                end;
                             end;
                              i_3:=j;
                             if (i_3>1) then
                             begin
                                while (i_3-1>=1) and (WhotisPole(i,i_3-1)<>cblack) do
                                begin
                                   dec(i_3);
                                    CopyList();
                                   if (WhotisPoledetector(i,i_3)=cwhite) then DeleteFigdetector(i,i_3);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i;
                                   arrbdetector[i_1].j:=i_3;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                   vacantmove[i,i_3]:=true;

                                   end;
                                   if (WhotisPole(i,i_3)=cwhite) then break;
                                end;
                             end;
                             i_3:=i;
                             if (i_3<8) then
                             begin
                                while (i_3+1<=8) and (WhotisPole(i_3+1,j)<>cblack) do
                                begin
                                   inc(i_3);
                                     CopyList();
                                   if (WhotisPoledetector(i_3,j)=cwhite) then DeleteFigdetector(i_3,j);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_3;
                                   arrbdetector[i_1].j:=j;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_3,j]:=true;
                                   end;
                                   if (WhotisPole(i_3,j)=cwhite) then break;
                                end;
                             end;
                               i_3:=i;
                             if (i_3>1) then
                             begin
                                while (i_3-1>=1) and (WhotisPole(i_3-1,j)<>cblack) do
                                begin
                                   dec(i_3);
                                    CopyList();
                                   if (WhotisPoledetector(i_3,j)=cwhite) then DeleteFigdetector(i_3,j);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_3;
                                   arrbdetector[i_1].j:=j;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_3,j]:=true;
                                   end;
                                   if (WhotisPole(i_3,j)=cwhite) then break;
                                end;
                             end;

                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2<8) then
                             begin
                                while (i_2+1<=8)and(j_2+1<=8) and (WhotisPole(i_2+1,j_2+1)<>cblack) do
                                begin
                                   inc(i_2);
                                   inc(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_2;
                                   arrbdetector[i_1].j:=j_2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                   if (WhotisPole(i_2,j_2)=cwhite) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2>1) then
                             begin
                                while (i_2-1>=1)and(j_2-1>=1) and (WhotisPole(i_2-1,j_2-1)<>cblack) do
                                begin
                                   dec(i_2);
                                   dec(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_2;
                                   arrbdetector[i_1].j:=j_2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                   if (WhotisPole(i_2,j_2)=cwhite) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2<8) then
                             begin
                                while (i_2-1>=1) and (j_2+1<=8) and (WhotisPole(i_2-1,j_2+1)<>cblack) do
                                begin
                                   dec(i_2);
                                   inc(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_2;
                                   arrbdetector[i_1].j:=j_2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                   if (WhotisPole(i_2,j_2)=cwhite) then break;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2>1) then
                             begin
                                while (i_2+1<=8)and(j_2-1>=1) and (WhotisPole(i_2+1,j_2-1)<>cblack) do
                                begin
                                   inc(i_2);
                                   dec(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_2;
                                   arrbdetector[i_1].j:=j_2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                   if (WhotisPole(i_2,j_2)=cwhite) then break;
                                end;
                             end;
                          end;

                          if (arrb[i_1].fig=cking) then
                          begin
                             // Ходы Короля
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2<8) then
                             begin
                                if (i_2+1<=8)and(j_2+1<=8) and (WhotisPole(i_2+1,j_2+1)<>cblack) then
                                begin
                                   inc(i_2);
                                   inc(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_2;
                                   arrbdetector[i_1].j:=j_2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2<8) then
                             begin
                                if (i_2-1>=1)and(j_2+1<=8) and (WhotisPole(i_2-1,j_2+1)<>cblack) then
                                begin
                                   dec(i_2);
                                   inc(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_2;
                                   arrbdetector[i_1].j:=j_2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (j_2<8) then
                             begin
                                if (j_2+1<=8) and (WhotisPole(i_2,j_2+1)<>cblack) then
                                begin
                                   inc(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_2;
                                   arrbdetector[i_1].j:=j_2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                end;
                             end;

                             i_2:=i;
                             j_2:=j;
                             if (i_2<8)and(j_2>1) then
                             begin
                                if (i_2+1<=8)and(j_2-1>=1) and (WhotisPole(i_2+1,j_2-1)<>cblack) then
                                begin
                                   inc(i_2);
                                   dec(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_2;
                                   arrbdetector[i_1].j:=j_2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1)and(j_2>1) then
                             begin
                                if (i_2-1>=1)and(j_2-1>=1) and (WhotisPole(i_2-1,j_2-1)<>cblack) then
                                begin
                                   dec(i_2);
                                   dec(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_2;
                                   arrbdetector[i_1].j:=j_2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (j_2>1) then
                             begin
                                if (j_2-1>=1) and (WhotisPole(i_2,j_2-1)<>cblack) then
                                begin
                                   dec(j_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_2;
                                   arrbdetector[i_1].j:=j_2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2<8) then
                             begin
                                if (i_2+1<=8)and (WhotisPole(i_2+1,j_2)<>cblack) then
                                begin
                                   inc(i_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_2;
                                   arrbdetector[i_1].j:=j_2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                end;
                             end;
                             i_2:=i;
                             j_2:=j;
                             if (i_2>1) then
                             begin
                                if (i_2-1>=1)and (WhotisPole(i_2-1,j_2)<>cblack) then
                                begin
                                   dec(i_2);
                                    CopyList();
                                   if (WhotisPoledetector(i_2,j_2)=cwhite) then DeleteFigdetector(i_2,j_2);  // скушали фигурку.
                                   arrbdetector[i_1].i:=i_2;
                                   arrbdetector[i_1].j:=j_2;
                                   GenerateWhiteAttack();
                                   if (not(BlackKing_Check(Sender))) then
                                   begin
                                      vacantmove[i_2,j_2]:=true;
                                   end;
                                end;
                             end;

                             // Рокировка в короткую сторону
                             if ((WhotisPole(1,6)=cempty)and
                             (WhotisPole(1,7)=cempty)and
                             (WhotisPole(1,8)=cblack)and
                             (WhotisPole(1,5)=cblack)and
                             (WhotisFig(1,8)=crook)and
                             (WhotisFig(1,5)=cking)) then
                             begin
                                 i_2:=i;
                                 j_2:=j;
                                 if (j_2+2<=8) then
                                 begin
                                    CopyList();
                                    GenerateWhiteAttack();
                                    if ((white_attack[1,6])or(white_attack[1,7])or
                                    (white_attack[1,5])or did_the_black_right_rook_move
                                    or did_the_black_king_move) then
                                    begin
                                       // рокировка невозможна.
                                       // если поле, пройденное королём во время рокировки, находится под боем противника;
                                       //если король перед началом рокировки находится под шахом или после её осуществления попадает под шах;
                                       // ходила правая ладья или уже ходил король.
                                    end
                                    else
                                    begin
                                       vacantmove[i_2,j_2+2]:=true;
                                    end;
                                 end;
                             end;

                             // Рокировка в длинную сторону
                             if ((WhotisPole(1,2)=cempty)and
                             (WhotisPole(1,3)=cempty)and
                             (WhotisPole(1,4)=cempty)and
                             (WhotisPole(1,1)=cblack)and
                             (WhotisPole(1,5)=cblack)and
                             (WhotisFig(1,1)=crook)and
                             (WhotisFig(1,5)=cking)) then
                             begin
                                 i_2:=i;
                                 j_2:=j;
                                 if (j_2-2>=1) then
                                 begin
                                    CopyList();
                                    GenerateWhiteAttack();
                                    if ((white_attack[1,3])or(white_attack[1,4])or
                                    (white_attack[1,5])or did_the_black_left_rook_move
                                    or did_the_black_king_move) then
                                    begin
                                       // рокировка невозможна.
                                       // если поле, пройденное королём во время рокировки, находится под боем противника;
                                       //если король перед началом рокировки находится под шахом или после её осуществления попадает под шах;
                                       // ходила левая ладья или уже ходил король.
                                    end
                                    else
                                    begin
                                       vacantmove[i_2,j_2-2]:=true;
                                    end;
                                 end;
                             end;

                          end;

                       end;
                    end;
                 end;

              end;
          end;
    end;
end;

procedure TForm1.FormPaint(Sender: TObject);
begin
   Draw(Sender);
end;

// Откат ровно на один ход назад.
procedure TForm1.MenuItem3Click(Sender: TObject);
var i,j:Integer;
    found : Boolean;
begin

   if (State<2) then
   begin

   for i:=1 to 8 do
       for j:=1 to 8 do
           vacantmove[i][j]:=false;




   if (cMove=cwhite) then
   begin
      // Предыдущий ход был чёрных
      SetLength(arrb,Length(arrb1));
      for i:=0 to High(arrb1) do
      begin
         arrb[i]:=arrb1[i];
      end;

      if (white_eating.fig<>cemptyfig) then
      begin
         // Возвращаем съеденую фигуру.
         found:=false;
         for i:=0 to High(arrw) do
         begin
            if ((not(found))and (arrw[i].fig=cemptyfig)) then
            begin
                arrw[i]:=white_eating;
                found:=true;
            end;
         end;
         if (not(found)) then
         begin
            SetLength(arrw,Length(arrw)+1);
            arrw[High(arrw)]:=white_eating;
         end;
         //DeleteFig1(white_eating.i,white_eating.j);
         //SetLength(arrw1,Length(arrw1)+1);
         //arrw1[High(arrw1)]:=white_eating;
      end;

      did_the_black_right_rook_move:=did_the_black_right_rook_move1;
      did_the_black_left_rook_move :=did_the_black_left_rook_move1; // ходили ли чёрные ладьи
      did_the_black_king_move:=did_the_black_king_move1; // ходили ли короли.

      cMove:=cblack;
   end
   else
   begin
      SetLength(arrw,Length(arrw1));
      for i:=0 to High(arrw1) do
      begin
         arrw[i]:=arrw1[i];
      end;

      if (black_eating.fig<>cemptyfig) then
      begin
         // Возвращаем съеденую фигуру.
         found:=false;
         for i:=0 to High(arrb) do
         begin
            if ((not(found))and (arrb[i].fig=cemptyfig)) then
            begin
                arrb[i]:=black_eating;
                found:=true;
            end;
         end;
         if (not(found)) then
         begin
            SetLength(arrb,Length(arrb)+1);
            arrb[High(arrb)]:=black_eating;
         end;
         //DeleteFig1(black_eating.i,black_eating.j);
         //SetLength(arrb1,Length(arrb1)+1);
         //arrb1[High(arrb1)]:=black_eating;
      end;

      did_the_white_right_rook_move:=did_the_white_right_rook_move1;
      did_the_white_left_rook_move :=did_the_white_left_rook_move1; // ходили ли белые ладьи
       did_the_white_king_move:=did_the_white_king_move1;

      cMove:=cwhite;
   end;
   {SetLength(arrw,Length(arrw1));
   SetLength(arrb,Length(arrb1));

   for i:=0 to High(arrw1) do
   begin
      arrw[i]:=arrw1[i];
   end;
   for i:=0 to High(arrb1) do
   begin
      arrb[i]:=arrb1[i];
   end;}


   end;

   if (State<2) then inc(State);
   if (State>2) then State:=2;
end;



procedure TForm1.Draw(Sender: TObject);
const n=8;
      kr=clTeal; // цвет темных клеток
      kw=clSilver; // цвет светлых клеток
      uclr=clWhite; // Цвет белых фигур.
var i,j:Integer;
begin
for i:=1 to n do
for j:=1 to n do
begin
   if (bPress and ((xmouse>40+(j-1)*60)and(xmouse<40+(j-1)*60+60)and(
   ymouse>20+(i-1)*60)and(ymouse<20+(i-1)*60+60))) then
   begin
      // Подсвечиваем какой фигурой пошли.
      Kletka(40+(j-1)*60,20+(i-1)*60,60,clYellow);
   end
   else if  (bPress and (vacantmove[i,j] or vacantmove1[i,j])) then
   begin
      // Подсвечиваем клетки куда можно пойти фигурой.
      if (vacantmove1[i,j]) then
      begin
          // Поля атаки.
          Kletka(40+(j-1)*60,20+(i-1)*60,60,clRed);
      end
       else
      begin
         if odd(i+j) then  Kletka(40+(j-1)*60,20+(i-1)*60,60,clMoneyGreen)
         else Kletka(40+(j-1)*60,20+(i-1)*60,60,clAqua);
      end;
   end
   else
   begin
      if odd(i+j) then Kletka(40+(j-1)*60,20+(i-1)*60,60,kr)
      else Kletka(40+(j-1)*60,20+(i-1)*60,60,kw);
   end;
end;
with Canvas do
 begin
  pen.Color:=kr;
  pen.Width:=3;
  brush.Style:=bsClear;
  rectangle(40,20,520,500);
font.Size:=12;
font.Style:=[fsBold];
for i:=1 to n do
 begin
  textout(20,i*60-20,inttostr(n-i+1));
  textout(i*60+10,510,chr(i+96));
 end;
end;


for i:=0 to High(arrw) do
begin
 case arrw[i].fig of
 cpawn : begin
    if (odd(arrw[i].i+arrw[i].j)) then
    begin
        Pawn(40+(arrw[i].j-1)*60,20+(arrw[i].i-1)*60,60,uclr,kr);
    end
    else
    begin
       Pawn(40+(arrw[i].j-1)*60,20+(arrw[i].i-1)*60,60,uclr,kw);
    end;
 end;
 crook : begin
    if (odd(arrw[i].i+arrw[i].j)) then
    begin
        Rook(40+(arrw[i].j-1)*60,20+(arrw[i].i-1)*60,60,uclr,kr);
    end
    else
    begin
       Rook(40+(arrw[i].j-1)*60,20+(arrw[i].i-1)*60,60,uclr,kw);
    end;
 end;
 cknight : begin
    if (odd(arrw[i].i+arrw[i].j)) then
    begin
        Knight(40+(arrw[i].j-1)*60,20+(arrw[i].i-1)*60,60,uclr,kr);
    end
    else
    begin
       Knight(40+(arrw[i].j-1)*60,20+(arrw[i].i-1)*60,60,uclr,kw);
    end;
 end;
 cbishop : begin
    if (odd(arrw[i].i+arrw[i].j)) then
    begin
        Bishop(40+(arrw[i].j-1)*60,20+(arrw[i].i-1)*60,60,uclr,kr);
    end
    else
    begin
       Bishop(40+(arrw[i].j-1)*60,20+(arrw[i].i-1)*60,60,uclr,kw);
    end;
 end;
 cqueen : begin
    if (odd(arrw[i].i+arrw[i].j)) then
    begin
        Queen(40+(arrw[i].j-1)*60,20+(arrw[i].i-1)*60,60,uclr,kr);
    end
    else
    begin
       Queen(40+(arrw[i].j-1)*60,20+(arrw[i].i-1)*60,60,uclr,kw);
    end;
 end;
 cking : begin
    if (odd(arrw[i].i+arrw[i].j)) then
    begin
        King(40+(arrw[i].j-1)*60,20+(arrw[i].i-1)*60,60,uclr,kr);
    end
    else
    begin
       King(40+(arrw[i].j-1)*60,20+(arrw[i].i-1)*60,60,uclr,kw);
    end;
 end;
 end;
end;

for i:=0 to High(arrb) do
begin
 case arrb[i].fig of
 cpawn : begin
    if (odd(arrb[i].i+arrb[i].j)) then
    begin
        Pawn(40+(arrb[i].j-1)*60,20+(arrb[i].i-1)*60,60,clBlack,kr);
    end
    else
    begin
       Pawn(40+(arrb[i].j-1)*60,20+(arrb[i].i-1)*60,60,clBlack,kw);
    end;
 end;
 crook : begin
    if (odd(arrb[i].i+arrb[i].j)) then
    begin
        Rook(40+(arrb[i].j-1)*60,20+(arrb[i].i-1)*60,60,clBlack,kr);
    end
    else
    begin
       Rook(40+(arrb[i].j-1)*60,20+(arrb[i].i-1)*60,60,clBlack,kw);
    end;
 end;
 cknight : begin
    if (odd(arrb[i].i+arrb[i].j)) then
    begin
        Knight(40+(arrb[i].j-1)*60,20+(arrb[i].i-1)*60,60,clBlack,kr);
    end
    else
    begin
       Knight(40+(arrb[i].j-1)*60,20+(arrb[i].i-1)*60,60,clBlack,kw);
    end;
 end;
 cbishop : begin
    if (odd(arrb[i].i+arrb[i].j)) then
    begin
        Bishop(40+(arrb[i].j-1)*60,20+(arrb[i].i-1)*60,60,clBlack,kr);
    end
    else
    begin
       Bishop(40+(arrb[i].j-1)*60,20+(arrb[i].i-1)*60,60,clBlack,kw);
    end;
 end;
 cqueen : begin
    if (odd(arrb[i].i+arrb[i].j)) then
    begin
        Queen(40+(arrb[i].j-1)*60,20+(arrb[i].i-1)*60,60,clBlack,kr);
    end
    else
    begin
       Queen(40+(arrb[i].j-1)*60,20+(arrb[i].i-1)*60,60,clBlack,kw);
    end;
 end;
 cking : begin
    if (odd(arrb[i].i+arrb[i].j)) then
    begin
        King(40+(arrb[i].j-1)*60,20+(arrb[i].i-1)*60,60,clBlack,kr);
    end
    else
    begin
       King(40+(arrb[i].j-1)*60,20+(arrb[i].i-1)*60,60,clBlack,kw);
    end;
 end;
 end;
end;

{i:=7;
for j:=1 to n do
begin
   if odd(j) then
   begin
      Pawn(40+(j-1)*60,20+(i-1)*60,60,uclr,kw);
   end
    else
   begin
      Pawn(40+(j-1)*60,20+(i-1)*60,60,uclr,kr);
   end;
end;
i:=8;
j:=1;
Rook(40+(j-1)*60,20+(i-1)*60,60,uclr,kr);
j:=8;
Rook(40+(j-1)*60,20+(i-1)*60,60,uclr,kw);
j:=4;
Queen(40+(j-1)*60,20+(i-1)*60,60,uclr,kw);
j:=5;
King(40+(j-1)*60,20+(i-1)*60,60,uclr,kr);
j:=3;
Bishop(40+(j-1)*60,20+(i-1)*60,60,uclr,kr);
j:=6;
Bishop(40+(j-1)*60,20+(i-1)*60,60,uclr,kw);
j:=2;
Knight(40+(j-1)*60,20+(i-1)*60,60,uclr,kw);
j:=7;
Knight(40+(j-1)*60,20+(i-1)*60,60,uclr,kr);
i:=2;
for j:=1 to n do
begin
   if odd(j) then
   begin
      Pawn(40+(j-1)*60,20+(i-1)*60,60,clBlack,kr);
   end
    else
   begin
      Pawn(40+(j-1)*60,20+(i-1)*60,60,clBlack,kw);
   end;
end;
i:=1;
j:=1;
Rook(40+(j-1)*60,20+(i-1)*60,60,clBlack,kw);
j:=8;
Rook(40+(j-1)*60,20+(i-1)*60,60,clBlack,kr);
j:=4;
Queen(40+(j-1)*60,20+(i-1)*60,60,clBlack,kr);
j:=5;
King(40+(j-1)*60,20+(i-1)*60,60,clBlack,kw);
j:=3;
Bishop(40+(j-1)*60,20+(i-1)*60,60,clBlack,kw);
j:=6;
Bishop(40+(j-1)*60,20+(i-1)*60,60,clBlack,kr);
j:=2;
Knight(40+(j-1)*60,20+(i-1)*60,60,clBlack,kr);
j:=7;
Knight(40+(j-1)*60,20+(i-1)*60,60,clBlack,kw);  }
end;

end.

