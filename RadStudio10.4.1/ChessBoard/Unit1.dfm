object Form1: TForm1
  Left = 192
  Top = 124
  Caption = 'chess'
  ClientHeight = 264
  ClientWidth = 330
  Color = clBtnFace
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  OnMouseDown = FormMouseDown
  OnPaint = FormPaint
  PixelsPerInch = 96
  TextHeight = 13
  object Timer1: TTimer
    Left = 16
    Top = 16
  end
  object MainMenu1: TMainMenu
    Left = 72
    Top = 16
    object File1: TMenuItem
      Caption = 'File'
      object Save1: TMenuItem
        Caption = 'Save'
        OnClick = Save1Click
      end
      object Open1: TMenuItem
        Caption = 'Open'
        OnClick = Open1Click
      end
      object Close1: TMenuItem
        Caption = 'Close'
        OnClick = Close1Click
      end
    end
    object Edit1: TMenuItem
      Caption = 'Edit'
      object Undo1: TMenuItem
        Caption = 'Undo'
        OnClick = Undo1Click
      end
      object Nextmove1: TMenuItem
        Caption = 'Next move'
        OnClick = Nextmove1Click
      end
      object picturefigures1: TMenuItem
        Caption = 'picture figures'
        OnClick = picturefigures1Click
      end
      object setemptyboard1: TMenuItem
        Caption = 'set empty board'
        OnClick = setemptyboard1Click
      end
      object rotatechessboard1: TMenuItem
        Caption = 'rotate chess board'
        OnClick = rotatechessboard1Click
      end
    end
  end
  object SaveDialog1: TSaveDialog
    Left = 136
    Top = 16
  end
  object OpenDialog1: TOpenDialog
    Left = 200
    Top = 16
  end
end
