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
    Image4: TImage;
    Image5: TImage;
    Label_info: TLabel;
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
      Image4.Visible:=false;
      Image5.Visible:=false;
      Label_info.Caption:='Два полухода без форсированного варианта.';
    end;
  1 : // Клод Шеннон
    begin
      // Первый уровень игры. В честь Клода Шеннона. Четыре полухода без форсированного варианта.
      // Только четыре полухода без форсированого варианта.
      Image1.Visible:=false;
      Image2.Visible:=true;
      Image3.Visible:=false;
      Image4.Visible:=false;
      Image5.Visible:=false;
      Label_info.Caption:='Четыре полухода без форсированного варианта. ';
    end;
  2 : // Джон фон Нейман
    begin
      // Второй уровень игры.  Два полухода с форсированным вариантом.
      // Только два полухода с форсированным вариантом.
      Image1.Visible:=false;
      Image2.Visible:=false;
      Image3.Visible:=false;
      Image4.Visible:=true;
      Image5.Visible:=false;
      Label_info.Caption:='Два полухода с форсированным вариантом.';
    end;
  3 : // Кен Томпсон
    begin
      // В следующих четырёх предложениях описывается ситуация для варианта без продлений взятий и шахов.
      // Шесть полуходов это неприемлемо долго для моего неоптимизированного кода солвера.
      // Ответ одного хода может затянутся на 4минуты, поэтому в коде применено отсечение некоторых ветвей дерева без
      // какого либо их рассмотрения на основе сортировки ходов. Это неверно так как алгоритм допускает тривиальные ошибки.
      // Подставляется под взятия. Надо считать до конца.
      // Третий уровень игры.  Шесть полуходов c форсированным вариантом.
      // Только  шесть полуходов с форсированым вариантом.
      // В действительности интересен только форсированный вариант иначе программа просто не видит разменов и тривиально проигрывает.
      // После включения форсажа alpha - beta чрезвычайно сильно ускорилась вероятно потому что  отсечения заработали особенно хорошо.
      // Вариант не кастированный а полный и время расчёта приемлемо. несколько секунд.
      Image1.Visible:=false;
      Image2.Visible:=false;
      Image3.Visible:=false;
      Image4.Visible:=false;
      Image5.Visible:=true;
      Label_info.Caption:='Шесть полуходов с форсированным вариантом. StockFish level 2. ELO 950.';
    end;
  4 : // Каиса Адельсон-Вельский, Арлазоров  ...
    // Это разумеется не сама Каисса это просто название тут.
    begin
      // Четвёртый (наивысший 13.09.2022) уровень игры. В честь советской программы Каисса и первого чемпионата мира среди машин.
      //  Четыре полухода с форсированным вариантом.
      // Только четыре полухода с форсированым вариантом. На фотографии Михаил Донской.
      Image1.Visible:=false;
      Image2.Visible:=false;
      Image3.Visible:=true;
      Image4.Visible:=false;
      Image5.Visible:=false;
      Label_info.Caption:='Четыре полухода с форсированным вариантом. StockFish level 2. ELO 950.';
    end;
  end;
end;

end.

