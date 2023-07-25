program Project1;

uses
  Forms,
  Unit1 in 'Unit1.pas' {Form1},
  Unitcompletechessstate in 'Unitcompletechessstate.pas' {FormCompleateCreationChessStudy},
  Unitnewfigure in 'Unitnewfigure.pas' {FormNewFigure};

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.CreateForm(TFormCompleateCreationChessStudy, FormCompleateCreationChessStudy);
  Application.CreateForm(TFormNewFigure, FormNewFigure);
  Application.Run;
end.
