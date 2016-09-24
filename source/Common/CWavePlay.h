#pragma once

#define PCM 0x0001
#pragma pack (push, 1)
typedef struct _WAVE_HEADER
{
	ULONG	Riff;				// 4 byte R' 'I' 'F' 'F' RIFFヘッダ
	ULONG	Length;				// 4 byte これ以降のファイルサイズ (ファイルサイズ - 8) 　 　
	ULONG	Wave;				// 4 byte W' 'A' 'V' 'E' WAVEヘッダ RIFFの種類がWAVEであることをあらわす 
	ULONG	chunkID;			// 4 byte f' 'm' 't' ' ' (←スペースも含む) fmt チャンク フォーマットの定義
	ULONG	chunkSize;			// 4 byte バイト数 fmt チャンクのバイト数 リニアPCM ならば 16(10 00 00 00)
	SHORT	wFromatID;			// 2 byte フォーマットID 参照 リニアPCM ならば 1(01 00) 
	SHORT	wChannels;			// 2 byte チャンネル数 　 モノラル ならば 1(01 00) ステレオ ならば 2(02 00) 
	ULONG	dwSamplesPerSec;	// 4 byte サンプリングレート Hz 44.1kHz ならば 44100(44 AC 00 00) 
	ULONG	dwDataRate;			// 4 byte データ速度 (Byte/sec) 　 44.1kHz 16bit ステレオ ならば44100×2×2 = 176400(10 B1 02 00) 
	SHORT	wBlockAlign;		// 2 byte ブロックサイズ (Byte/sample×チャンネル数) 　 16bit ステレオ ならば2×2 = 4(04 00) 
	SHORT	wBitsPerSample;		// 2 byte サンプルあたりのビット数 (bit/sample) WAV フォーマットでは 8bit か 16bit 16bit ならば 16(10 00) 
	ULONG	DataChunk;			// 4 byte d' 'a' 't' 'a' data チャンク 参照 　 
	ULONG	DataChunkLength;	// 4 byte バイト数n 波形データのバイト数 　 
}WAVE_HEADER, *PWAVE_HEADER;
#pragma pack(pop)


class CWavePlay
{
public:
	CWavePlay(void);
	~CWavePlay(void);

	VOID Load (HINSTANCE hInstance, ULONG Index, ULONG ResID, ULONG Count);
	VOID Play (LPCSTR pszString);
	VOID Stop ();
	VOID SetVolume (ULONG Level);
	ULONG GetVolume ();

private:
	PWAVE_HEADER LoadWave (HINSTANCE hInstance, UINT Id);
	PWAVE_HEADER Init (ULONG Index);
	PWAVE_HEADER Append (ULONG Index);
	PWAVE_HEADER VolumeApply ();

private:
	CMap<ULONG, ULONG, PWAVE_HEADER, PWAVE_HEADER> m_Map;
	PWAVE_HEADER	m_pWaveBuffer;
	ULONG			m_Level;

	enum
	{
		WAVE_BUFFER_SIZE = 8 * 1024 * 1024,
	};
};
