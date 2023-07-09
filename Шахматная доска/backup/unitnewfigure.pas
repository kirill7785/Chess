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

end.

