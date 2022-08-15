module clock (led5,led4,led3,led2,led1,led0,clk400_out,
      sub_dot,sub_led,sub_2cl,
      rst_n,mclk,
      clk16m_in,ts_n,hs_n,ms_n,test_n );

  output [6:0] led5;           // ���v�̎���2���ڂ�LED�\���o��
  output [6:0] led4;           // ���v�̎���1���ڂ�LED�\���o��
  output [6:0] led3;           // ���v�̕�2���ڂ�LED�\���o��
  output [6:0] led2;           // ���v�̕�1���ڂ�LED�\���o��
  output [6:0] led1;           // ���v�̕b2���ڂ�LED�\���o��
  output [6:0] led0;           // ���v�̕b1���ڂ�LED�\���o��
  output       clk400_out;     // 16MHz�N���b�N�𕪎�����400Hz�o��

  output [5:0] sub_dot;        //�\��
  output [7:0] sub_led;        //�\��
  output [7:0] sub_2cl;        //�\��

  input        rst_n;          // �{�{�[�h�S�̂̃V�X�e�����Z�b�g
  input        mclk;           // �{�{�[�h�̃V�X�e���N���b�N
  input        clk16m_in;      // ���U��H�Ő������ꂽ16MHz�N���b�N
  input        ts_n;           // �����ݒ�SW����
  input        hs_n;           // ���ԑ���SW����
  input        ms_n;           // ������SW����
  input        test_n;         // �e�X�g���[�hSW���͐M��

endmodule
