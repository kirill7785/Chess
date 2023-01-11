unit unitBotSettings;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, ExtCtrls;

type

  { TFormBotSettings }

  TFormBotSettings = class(TForm)
    ComboBoxBotOpponent: TComboBox;
    GroupBox1: TGroupBox;
    Image1: TImage;
    Image2: TImage;
    Image3: TImage;
    procedure ComboBoxBotOpponentChange(Sender: TObject);
  private

  public

  end;

var
  FormBotSettings: TFormBotSettings;

implementation

{$R *.lfm}

{ TFormBotSettings }

procedure TFormBotSettings.ComboBoxBotOpponentChange(Sender: TObject);
begin
  case ComboBoxBotOpponent.ItemIndex of
  0 : // Алан Тьюринг
    begin
      // Нулевой уровень игры. Бумажная машина Алана Тьюринга. Два полухода без форсированного варианта.
      // Только два полухода без форсированого варианта, пропускает детский мат.
      Image1.Visible:=true;
      Image2.Visible:=false;
      Image3.Visible:=false;
    end;
  1 : // Клод Шеннон
    begin
      // Первый уровень игры. В честь Клода Шеннона. Четыре полухода без форсированного варианта.
      // Только четыре полухода без форсированого варианта.
      Image1.Visible:=false;
      Image2.Visible:=true;
      Image3.Visible:=false;
    end;
  2 : // Каиса Адельсон-Вельский, Арлазоров  ...
    // Это разумеется не сама Каисса это просто название тут.
    begin
      // Второй (наивысший 13.09.2022) уровень игры. В честь советской программы Каисса и первого чемпионата мира среди машин.
      //  Четыре полухода с формированным вариантом.
      // Только четыре полухода с форсированым вариантом. На фотографии Михаил Донской.
      Image1.Visible:=false;
      Image2.Visible:=false;
      Image3.Visible:=true;
    end;
  end;
end;

end.

