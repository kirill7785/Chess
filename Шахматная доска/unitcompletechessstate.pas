unit unitCompleteChessState;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, StdCtrls;

type

  { TFormCompleateCreationChessStudy }

  TFormCompleateCreationChessStudy = class(TForm)
    Button1: TButton;
    RadioGroup1: TRadioGroup;
    RadioGroup2: TRadioGroup;
    RadioGroup3: TRadioGroup;
    RadioGroup4: TRadioGroup;
    RadioGroup5: TRadioGroup;
    RadioGroup6: TRadioGroup;
    RadioGroup7: TRadioGroup;
    procedure Button1Click(Sender: TObject);
  private

  public

  end;

var
  FormCompleateCreationChessStudy: TFormCompleateCreationChessStudy;

implementation

{$R *.lfm}

{ TFormCompleateCreationChessStudy }

procedure TFormCompleateCreationChessStudy.Button1Click(Sender: TObject);
begin
   Close;
end;

end.

