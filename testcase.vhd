-- $Id: mini_aes.vhdl,v 1.2 2005-12-23 04:21:39 arif_endro Exp $
-------------------------------------------------------------------------------
-- Title       : Mini AES 128
-- Project     : Mini AES 128 
-------------------------------------------------------------------------------
-- File        : mini_aes.vhdl
-- Author      : "Arif E. Nugroho" <arif_endro@yahoo.com>
-- Created     : 2005/12/03
-- Last update : 
-- Simulators  : ModelSim SE PLUS 6.0
-- Synthesizers: ISE Xilinx 6.3i
-- Target      : 
-------------------------------------------------------------------------------
-- Description : Mini AES 128 top component
-------------------------------------------------------------------------------
-- Copyright (C) 2005 Arif E. Nugroho
-- This VHDL design file is an open design; you can redistribute it and/or
-- modify it and/or implement it after contacting the author
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-- 
--         THIS SOURCE FILE MAY BE USED AND DISTRIBUTED WITHOUT RESTRICTION
-- PROVIDED THAT THIS COPYRIGHT STATEMENT IS NOT REMOVED FROM THE FILE AND THAT
-- ANY DERIVATIVE WORK CONTAINS THE ORIGINAL COPYRIGHT NOTICE AND THE
-- ASSOCIATED DISCLAIMER.
-- 
-------------------------------------------------------------------------------
-- 
--         THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
-- IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
-- MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
-- EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
-- SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
-- PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
-- OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
-- WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
-- OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
-- ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-- 
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity mini_aes is
  port (
    clock  : in  std_logic;
    clear  : in  std_logic;
    load_i : in  std_logic;
    enc    : in  std_logic;             -- active low (e.g. 0 = encrypt, 1 = decrypt)
    key_i  : in  std_logic_vector (7 downto 0);
    data_i : in  std_logic_vector (7 downto 0);
    data_o : out std_logic_vector (7 downto 0);
    done_o : out std_logic
    );
end mini_aes;

architecture data_flow of mini_aes is

  component io_interface
    port (
      clock      : in  std_logic;
      clear      : in  std_logic;
      load_i     : in  std_logic;
      load_i_int : out std_logic;
      data_i     : in  std_logic_vector (7 downto 0);
      key_i      : in  std_logic_vector (7 downto 0);
      data_o     : out std_logic_vector (7 downto 0);
      data_o_int : in  std_logic_vector (127 downto 000);
      data_i_int : out std_logic_vector (127 downto 000);
      key_i_int  : out std_logic_vector (127 downto 000);
      done_o_int : in  std_logic;
      done_o     : out std_logic
      );
  end component;

  component counter2bit
    port (
      clock       : in  std_logic;
      clear       : in  std_logic;
      count       : out std_logic_vector (1 downto 0)
      );
  end component;

  type state        is array (03 downto 00) of std_logic_vector (07 downto 00);
  type allround     is array (43 downto 00) of std_logic_vector (1 downto 00);
  type partialround is array (03 downto 00) of std_logic_vector (1 downto 00);
  signal   input            : state                             := ( X"00", X"00", X"00", X"00");
  signal   key_o_srl1_p     : partialround                      :=
    (
      "00", "00","00", "00"
      );
  signal   key_o_srl2_p     : partialround                      :=
    (
      "00", "00","00", "00"
      );
  signal   key_o_srl3_p     : partialround                      :=
    (
      "00", "00","00", "00"
      );
  signal   key_o_srl4_p     : partialround                      :=
    (
      "00", "00","00", "00"
      );
  signal   key_o_srl1       : allround                          :=
    (
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00"
      );
  signal   key_o_srl2       : allround                          :=
    (
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00"
      );
  signal   key_o_srl3       : allround                          :=
    (
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00",
      "00", "00", "00", "00"
      );
--
  signal   counter          : std_logic_vector (01 downto 00)     := "00";
  signal   inner_round      : std_logic                           := '0';
  signal   key_counter_up   : integer range 0 to 43;
  signal   key_counter_down : integer range 0 to 43;
  signal   done             : std_logic                           := '0';
  signal   done_decrypt     : std_logic                           := '0';
  signal   counter1bit      : std_logic                           := '0';
  signal   done_o_int       : std_logic                           := '0';
  signal   data_i_int       : std_logic_vector (127 downto 000)   := ( X"00000000_00000000_00000000_00000000" );
  signal   data_o_int       : std_logic_vector (127 downto 000)   := ( X"00000000_00000000_00000000_00000000" );
  signal   key_i_int        : std_logic_vector (127 downto 000)   := ( X"00000000_00000000_00000000_00000000" );
  signal   load             : std_logic                           := '0';
  signal   load_io          : std_logic                           := '0';
  signal   key_o            : std_logic_vector (031 downto 000)   := ( X"00000000" );
  signal   key_b            : std_logic_vector (127 downto 000)   := ( X"00000000_00000000_00000000_00000000" );
  constant GND              : std_logic                           := '0';
  constant VCC              : std_logic                           := '1';

  signal   inv_mixcol_o     : std_logic_vector (031 downto 000)   := ( X"00000000" );
--
  signal   en_a_i           : std_logic;
  signal   en_b_i           : std_logic;
  signal   clk_a_i          : std_logic;
  signal   clk_b_i          : std_logic;
  signal   we_a_i           : std_logic;
  signal   we_b_i           : std_logic;
  signal   di_a_i           : std_logic_vector (07 downto 00)   := B"0000_0000";
  signal   di_b_i           : std_logic_vector (07 downto 00)   := B"0000_0000";
  signal   addr_a_1_i       : std_logic_vector (08 downto 00)   := B"0_0000_0000";
  signal   addr_a_2_i       : std_logic_vector (08 downto 00)   := B"0_0000_0000";
  signal   addr_b_1_i       : std_logic_vector (08 downto 00)   := B"0_0000_0000";
  signal   addr_b_2_i       : std_logic_vector (08 downto 00)   := B"0_0000_0000";
  signal   do_a_1_o         : std_logic_vector (07 downto 00);
  signal   do_a_2_o         : std_logic_vector (07 downto 00);
  signal   do_b_1_o         : std_logic_vector (07 downto 00);
  signal   do_b_2_o         : std_logic_vector (07 downto 00);

--signal data_i_int : std_logic_vector (127 downto 000) :=
--( X"3243F6A8_885A308D_313198A2_E0370734" );  -- PT 0
--( X"00112233_44556677_8899AABB_CCDDEEFF" );  -- PT 1
--( X"3925841D_02DC09FB_DC118597_196A0B32" );  -- CT 0
--( X"69C4E0D8_6A7B0430_D8CDB780_70B4C55A" );  -- CT 1
--signal key_i : std_logic_vector (127 downto 000) :=
--( X"2B7E1516_28AED2A6_ABF71588_09CF4F3C" );  -- KEY 0
--( X"00010203_04050607_08090A0B_0C0D0E0F" );  -- KEY 1

begin

  clk_a_i <= clock;
  clk_b_i <= clock;
  en_a_i  <= VCC;
  en_b_i  <= VCC;
  we_a_i  <= GND;
  we_b_i  <= GND;

  done_o_int <= done_decrypt;

  my_io : io_interface
    port map (
      clock      => clock,
      clear      => clear,
      load_i     => load_i,
      load_i_int => load_io,
      data_i     => data_i,
      key_i      => key_i,
      data_o     => data_o,
      data_o_int => data_o_int,
      data_i_int => data_i_int,
      key_i_int  => key_i_int,
      done_o_int => done_o_int,
      done_o     => done_o
      );

  count2bit : counter2bit
    port map (
      clock       => clock,
      clear       => load,
      count       => counter
      );

  process(clear, clock)
  begin
    if (clear = '1') then
      key_o_srl1                  <= (others => (others => '0'));
    elsif (clock = '1' and clock'event) then
      if (inner_round = '1') then
        key_o_srl1 (43 downto 00) <= key_o_srl2 (43 downto 00);
      end if;
    end if;
  end process;
--
  process(clear, clock)
  begin
    if (clear = '1') then
      key_o_srl1_p                <= (others => (others => '0'));
    elsif (clock = '1' and clock'event) then
      key_o_srl1_p (03 downto 00) <= key_o_srl2_p (03 downto 00);
    end if;
  end process;
--
  process(clear, clock)
  begin
    if (clear = '1') then
      key_o_srl3_p                <= (others => (others => '0'));
    elsif (clock = '1' and clock'event) then
      key_o_srl3_p (03 downto 00) <= key_o_srl4_p (03 downto 00);
    end if;
  end process;

  key_o_srl2_p (03 downto 01) <= key_o_srl1_p (02 downto 00);
  key_o_srl2_p (00)           <= key_o(01 downto 00);
  key_o_srl4_p (02 downto 00) <= key_o_srl3_p (03 downto 01);
  key_o_srl4_p (03)           <= key_o(01 downto 00);
--
  inner_round <= ( counter(1) and counter(0) );
--
  key_o_srl2 (39 downto 00) <= key_o_srl1 (43 downto 04);

  key_o_srl2 (43 downto 40) <= ( key_o_srl4_p (03), key_o_srl4_p (02), key_o_srl4_p (01), key_o_srl4_p (00) ) when ( enc = '0' ) else 
                               ( key_o_srl2_p (03), key_o_srl2_p (02), key_o_srl2_p (01), key_o_srl2_p (00) );

  key_o_srl3 (43 downto 00) <= ( key_o_srl2 (43 downto 04) & 
                                 key_i_int (127 downto 096) & 
                                 key_i_int (095 downto 064) & 
                                 key_i_int (063 downto 032) & 
                                 key_i_int (031 downto 000)
                               ) when (done = '1') else 
                                 key_o_srl3 (43 downto 00);

end data_flow;
