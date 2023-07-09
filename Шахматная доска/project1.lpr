program project1;

{$mode objfpc}{$H+}

uses
  {$IFDEF UNIX}{$IFDEF UseCThreads}
  cthreads,
  {$ENDIF}{$ENDIF}
  Interfaces, // this includes the LCL widgetset
  Forms, Unit1, unitNewFigure, unitCompleteChessState
  { you can add units after this };

{$R *.res}

begin
  RequireDerivedFormResource := True;
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.CreateForm(TFormNewFigure, FormNewFigure);
  Application.CreateForm(TFormCompleateCreationChessStudy, 
    FormCompleateCreationChessStudy);
  Application.Run;
end.

