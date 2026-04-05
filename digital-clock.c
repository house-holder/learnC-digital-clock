#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "raylib.h"

#define windowWidth 690
#define windowHeight 180

#define segmentLen 50.0
#define segmentPadding 2.0
#define segmentThickness 14.0

#define drawColor RED

#define dWidth (segmentLen*2)						// digit width
#define cWidth (segmentLen-(segmentThickness/2))	// colon width
#define cInset (segmentLen/3)						// colon inset

#define segA (1 << 0)  // 0b00000001
#define segB (1 << 1)  // 0b00000010
#define segC (1 << 2)  // 0b00000100
#define segD (1 << 3)  // 0b00001000
#define segE (1 << 4)  // 0b00010000
#define segF (1 << 5)  // 0b00100000
#define segG (1 << 6)  // 0b01000000

uint8_t digitMap[10] = {
	0b00111111, // digit 0: ..FEDCBA
	0b00000110, // digit 1: .....CB.
	0b01011011, // digit 2: .G.ED.BA
	0b01001111, // digit 3: .G..DCBA
	0b01100110, // digit 4: .GF..CB.
	0b01101101, // digit 5: .GF.DC.A
	0b01111101, // digit 6: .GFEDC.A
	0b00000111, // digit 7: .....CBA
	0b01111111, // digit 8: .GFEDCBA
	0b01101111, // digit 9: .GF.DCBA
};

typedef struct {
	Vector2 points[6];
} Segment;

typedef struct {
	int hrs, min, sec;
} TimeHMS;

Vector2 GetSegmentCenter(char displayCode, Vector2 startPosition) {
	Vector2 p = startPosition;
	float T = segmentThickness, L = segmentLen; // aliases for declutter
	switch(displayCode) {
		case 'A': // top
			return (Vector2){p.x + T + L/2, p.y + T/2};
		case 'B': // upper right
			return (Vector2){p.x + T*1.5 + L, p.y + T + L/2};
		case 'C': // lower right
			return (Vector2){p.x + T*1.5 + L, p.y + T*2 + L*1.5};
		case 'D': // bottom
			return (Vector2){p.x + T + L/2, p.y + T*2.5 + L*2};
		case 'E': // lower left
			return (Vector2){p.x + T/2, p.y + T*2 + L*1.5};
		case 'F': // upper left
			return (Vector2){p.x + T/2, p.y + T + L/2};
		case 'G': // middle
			return (Vector2){p.x + T + L/2, p.y + T*1.5 + L};
		default:
			printf("Error: getSegmentCtr() default case");
			return p;
	}
}

Segment NewSegment(char displayCode, Vector2 startPosition) {
	Segment s;
	Vector2 ctr = GetSegmentCenter(displayCode, startPosition);
	float wt = segmentThickness, len = segmentLen - segmentPadding*2;

	if(displayCode == 'A' || displayCode == 'D' || displayCode == 'G') {
		s.points[0] = (Vector2){ctr.x - len/2 - wt/2, ctr.y};
		s.points[1] = (Vector2){ctr.x - len/2, ctr.y + wt/2};
		s.points[2] = (Vector2){ctr.x - len/2, ctr.y - wt/2};
		s.points[3] = (Vector2){ctr.x + len/2, ctr.y + wt/2};
		s.points[4] = (Vector2){ctr.x + len/2, ctr.y - wt/2};
		s.points[5] = (Vector2){ctr.x + len/2 + wt/2, ctr.y};
	} else {
		s.points[0] = (Vector2){ctr.x, ctr.y - len/2 - wt/2};
		s.points[1] = (Vector2){ctr.x - wt/2, ctr.y - len/2};
		s.points[2] = (Vector2){ctr.x + wt/2, ctr.y - len/2};
		s.points[3] = (Vector2){ctr.x - wt/2, ctr.y + len/2};
		s.points[4] = (Vector2){ctr.x + wt/2, ctr.y + len/2};
		s.points[5] = (Vector2){ctr.x, ctr.y + len/2 + wt/2};
	}
	return s;
}

void DrawSegment(char code, Vector2 origin) {
	Segment s = NewSegment(code, origin);
	DrawTriangleStrip(s.points, 6, drawColor);
}

void DrawDigit(int digit, Vector2 origin) {
	uint8_t mask = digitMap[digit];
	if (mask & segA) DrawSegment('A', origin);
	if (mask & segB) DrawSegment('B', origin);
	if (mask & segC) DrawSegment('C', origin);
	if (mask & segD) DrawSegment('D', origin);
	if (mask & segE) DrawSegment('E', origin);
	if (mask & segF) DrawSegment('F', origin);
	if (mask & segG) DrawSegment('G', origin);
}

void DrawColon(Vector2 pos) {
	float radius = segmentThickness / 2;
	float T = segmentThickness, L = segmentLen;
	float dotX = pos.x + T;
	DrawCircle(dotX, pos.y + T + L/2,		radius, drawColor);
	DrawCircle(dotX, pos.y + T*2 + L*1.5,	radius, drawColor);
}

TimeHMS GetLocalTime(void) {
	time_t t = time(NULL);
	struct tm *lt = localtime(&t);
	return (TimeHMS){ lt->tm_hour, lt->tm_min, lt->tm_sec };
}

void DrawTime(TimeHMS t) {
	float x = 20; float y = 20;

	DrawDigit(t.hrs/10, (Vector2){x, y});		x += dWidth;
	DrawDigit(t.hrs%10, (Vector2){x, y});		x += cWidth;
	DrawColon((Vector2){x+cWidth, y+cInset});	x += cWidth*2;

	DrawDigit(t.min/10, (Vector2){x, y});		x += dWidth;
	DrawDigit(t.min%10, (Vector2){x, y});		x += cWidth;
	DrawColon((Vector2){x+cWidth, y+cInset});	x += cWidth*2;

	DrawDigit(t.sec/10, (Vector2){x, y});		x += dWidth;
	DrawDigit(t.sec%10, (Vector2){x, y});
}

int main(void) {
	InitWindow(windowWidth, windowHeight, "Digital Clock");

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);

		TimeHMS t = GetLocalTime();
		DrawTime(t);

		EndDrawing();
	}

	return 0;
}
