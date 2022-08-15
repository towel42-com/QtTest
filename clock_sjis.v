module clock (led5,led4,led3,led2,led1,led0,clk400_out,
      sub_dot,sub_led,sub_2cl,
      rst_n,mclk,
      clk16m_in,ts_n,hs_n,ms_n,test_n );

  output [6:0] led5;           // 時計の時間2桁目のLED表示出力
  output [6:0] led4;           // 時計の時間1桁目のLED表示出力
  output [6:0] led3;           // 時計の分2桁目のLED表示出力
  output [6:0] led2;           // 時計の分1桁目のLED表示出力
  output [6:0] led1;           // 時計の秒2桁目のLED表示出力
  output [6:0] led0;           // 時計の秒1桁目のLED表示出力
  output       clk400_out;     // 16MHzクロックを分周した400Hz出力

  output [5:0] sub_dot;        //予備
  output [7:0] sub_led;        //予備
  output [7:0] sub_2cl;        //予備

  input        rst_n;          // 本ボード全体のシステムリセット
  input        mclk;           // 本ボードのシステムクロック
  input        clk16m_in;      // 発振回路で生成された16MHzクロック
  input        ts_n;           // 時刻設定SW入力
  input        hs_n;           // 時間送りSW入力
  input        ms_n;           // 分送りSW入力
  input        test_n;         // テストモードSW入力信号

endmodule
