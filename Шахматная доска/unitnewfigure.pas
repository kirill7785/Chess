unit unitNewFigure;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, StdCtrls;

type

  { TFormNewFigure }

  TFormNewFigure = class(TForm)
    Button1: TButton;
    RadioGroup1: TRadioGroup;
    RadioGroup2: TRadioGroup;
    RadioGroup3: TRadioGroup;
    procedure Button1Click(Sender: TObject);
    procedure RadioGroup1SelectionChanged(Sender: TObject);
  private

  public

  end;

var
  FormNewFigure: TFormNewFigure;

implementation

{$R *.lfm}

{ TFormNewFigure }

procedure TFormNewFigure.Button1Click(Sender: TObject);
begin
   Close;
end;

procedure TFormNewFigure.RadioGroup1SelectionChanged(Sender: TObject);
begin
   if (RadioGroup1.ItemIndex=0) then
   begin
      // continue
      RadioGroup2.Visible:=true;
      RadioGroup3.Visible:=true;
   end
   else
   begin
      // stop
      RadioGroup2.Visible:=false;
      RadioGroup3.Visible:=false;
   end;
end;



end.

