unit Unitnewfigure;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ExtCtrls;

type
  TFormNewFigure = class(TForm)
    RadioGroup1: TRadioGroup;
    RadioGroup2: TRadioGroup;
    RadioGroup3: TRadioGroup;
    FormNewFigure: TButton;
    procedure FormNewFigureClick(Sender: TObject);
    procedure RadioGroup1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  FormNewFigure: TFormNewFigure;

implementation

{$R *.dfm}

procedure TFormNewFigure.FormNewFigureClick(Sender: TObject);
begin
   Close;
end;

procedure TFormNewFigure.RadioGroup1Click(Sender: TObject);
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
