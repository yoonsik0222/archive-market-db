-- DROP TABLE
DROP TABLE IF EXISTS TRANSACTIONS;
DROP TABLE IF EXISTS APPRAISAL;
DROP TABLE IF EXISTS ITEM;
DROP TABLE IF EXISTS APPRAISER;
DROP TABLE IF EXISTS BRAND;
DROP TABLE IF EXISTS USER_INFO;

-- CREATE TABLE

CREATE TABLE USER_INFO (
	USER_ID INT NOT NULL,				-- USER_ID (1 : ADMIN)
	USER_NICKNAME VARCHAR(20) UNIQUE,
	USER_EMAIL VARCHAR(100) UNIQUE,
	USER_PASSWORD VARCHAR(20),
	-- 탈퇴 여부를 관리하는 컬럼 (0: 활동 중, 1: 탈퇴 비활성화)
    IS_DELETED TINYINT NOT NULL DEFAULT 0 CHECK (IS_DELETED IN (0, 1)),
	PRIMARY KEY (USER_ID)
);

CREATE TABLE BRAND (
	BRAND_ID INT NOT NULL,				-- BRAND_ID (0 : 없어진 브랜드 통칭)
	BRAND_NAME VARCHAR(20) UNIQUE,
	FOUNDATION_YEAR INT NOT NULL,
	PRIMARY KEY (BRAND_ID)
);

CREATE TABLE APPRAISER (
	APPRAISER_ID INT NOT NULL,				-- APPRAISER_ID (0 : 탈퇴한 감정사 통칭)
	APPI_NAME VARCHAR(20) NOT NULL,			-- 감정사 이름
	LICENSE_NO INT NOT NULL UNIQUE,			-- 감정사 자격증 번호
	SPECIALTY INT,							-- 주력 분야(브랜드)
	CAREER_YEARS INT NOT NULL,				-- 경력
	STATUS VARCHAR(20) DEFAULT 'ACTIVE' CHECK (STATUS IN ('ACTIVE', 'SUSPENDED', 'INACTIVE')),
	RELIABILITY INT DEFAULT 50 CHECK (RELIABILITY >= 0 AND RELIABILITY <= 100),

	--CONSTRAINTS
	PRIMARY KEY (APPRAISER_ID),
	FOREIGN KEY (SPECIALTY) REFERENCES BRAND(BRAND_ID)
		ON DELETE SET NULL ON UPDATE CASCADE
);

CREATE TABLE ITEM (
	ITEM_ID INT NOT NULL,
	MODEL_NAMES VARCHAR(100) NOT NULL,						-- 공식 모델 명
	POST_TITLE VARCHAR(200) NOT NULL,						-- 유저가 등록한 게시글 이름
	SEASON VARCHAR(20),										-- 상품 발매 시즌. 유저 등록. NULL OK
	ITEM_SIZE VARCHAR(10) NOT NULL,							-- 유저 인터페이스에서 등록한 사이즈, (S,M,L ... 0,1,2,.... 등등)
	USER_CONDITIONS VARCHAR(20) NOT NULL,					-- 유저가 등록한 상품 상태 (새상품/사용감 없음/약간의 오염/심한 오염)
	PRICE INT NOT NULL CHECK (PRICE > 0),					-- 상품 가격
	BUY_STATUS VARCHAR(20) NOT NULL,						-- 상품의 등록 상태 (판매중/판매완료/숨김)
	DESIGNER VARCHAR(50),									-- 유저가 등록한 상품 디자이너

	-- FOREIGN KEY SET
	BRAND_ID INT NOT NULL DEFAULT 0,						-- 아이템은 브랜드 아이템만 등록 할 수 있음.
	OWN_USER INT NOT NULL,
	ORIGINAL_MODEL INT,

	-- CONSTRAINTS
	PRIMARY KEY (ITEM_ID),
	FOREIGN KEY (OWN_USER) REFERENCES USER_INFO(USER_ID)
		ON DELETE NO ACTION ON UPDATE CASCADE,				-- IS_DELETED로 SOFT DELETION 이므로 DELETE시 NO ACTION.
	FOREIGN KEY (BRAND_ID) REFERENCES BRAND(BRAND_ID)
		ON DELETE SET DEFAULT ON UPDATE CASCADE,
	FOREIGN KEY (ORIGINAL_MODEL) REFERENCES ITEM(ITEM_ID)
		ON DELETE NO ACTION ON UPDATE NO ACTION
		-- 자기참조로 인한 다중 CASCADE 경로 방지.
		-- 원형 모델 삭제 시 ORIGINAL_MODEL NULL 처리는 애플리케이션 레벨에서 수행.
);


CREATE TABLE APPRAISAL (
	ITEM_ID INT NOT NULL,
	APPRAISAL_NO INT NOT NULL,
	VERIFIED_GRADE VARCHAR(20) NOT NULL,
	FINAL_RESULT VARCHAR(20) NOT NULL,
	COMMENT VARCHAR(255) NOT NULL,
	
	-- FOREIGEN KEY
	APPRAISER_ID INT NOT NULL DEFAULT 0,		-- 0 == '퇴사한 이전 감정사 통칭'

	-- CONSTARINTS
	PRIMARY KEY (ITEM_ID, APPRAISAL_NO),
	FOREIGN KEY (ITEM_ID) REFERENCES ITEM(ITEM_ID)
		ON DELETE CASCADE ON UPDATE CASCADE,
	FOREIGN KEY (APPRAISER_ID) REFERENCES APPRAISER(APPRAISER_ID)
		ON DELETE NO ACTION ON UPDATE NO ACTION
		-- BRAND_ID 변경시 ITEM, APPRAISAER 둘 다 변경되므로 우선 해당 정보 수정 경로 차단
);

CREATE TABLE TRANSACTIONS (
	TRADE_ID INT NOT NULL,
	PRICE INT NOT NULL CHECK (PRICE > 0),
	PAYMENT_METHOD VARCHAR(20) NOT NULL,
	TRANSACTION_DATE DATETIME NOT NULL DEFAULT GETDATE(),
	
	-- FOREIGN KEY
	ITEM_ID INT NOT NULL UNIQUE,			-- 모든 아이템에 관해 한 번의 거래 내역만 존재할 수 있으므로 UNIQUE
	SELLER_ID INT NOT NULL,
	BUYER_ID INT NOT NULL,

	-- CONSTARINTS
	PRIMARY KEY (TRADE_ID),
	FOREIGN KEY (ITEM_ID) REFERENCES ITEM(ITEM_ID)
		ON DELETE NO ACTION ON UPDATE CASCADE,
	FOREIGN KEY (SELLER_ID) REFERENCES USER_INFO(USER_ID)		-- SELLER와 BUYER 모두 USER_INFO 소속. 보통 USER_ID는 변경될 일이 없으므로.
		ON DELETE NO ACTION ON UPDATE NO ACTION,
	FOREIGN KEY (BUYER_ID) REFERENCES USER_INFO(USER_ID)
		ON DELETE NO ACTION ON UPDATE NO ACTION					-- 중복 경로가 발생. 상대적으로 덜 중요한 BUYER_ID를 NO ACTION
);


-- TUPLE INSERTION

PRINT '=======================================';
PRINT '          Insertion Tuples';
PRINT '=======================================';
GO

---- USER(USER_ID, USER_NICKNAME, USER_EMAIL, USER_PASSWORD, IS_DELETED(FLAG))
PRINT 'LEVEL 1 : USER_INFO';
INSERT INTO USER_INFO (USER_ID, USER_NICKNAME, USER_EMAIL, USER_PASSWORD, IS_DELETED) VALUES
(1,		'SYSTEM_ADMIN',		'admin@admin.com',				'1q2w3e4r!', 0),
(2,		'hype_beast',		'kimch0611@naver.com',			'rlacjftn61!', 0),
(3,		'vintageLover',		'leeseangyun12@gmail.com',		'actw041152!', 0),
(4,		'oneMore',			'lhw0619@gmail.com',			'gusdn1234@', 0),
(5,		'paf_fanboy',		'oh5252@naver.com',				'woghdtnqls5252~', 0),
(6,		'out_of_stock',		'kjh04231@naver.com',			'wngkehf999@a', 0),
(7,		'gallery_look',		'jeonkayeong333@gmail.com',		'1q2w3e4r!', 0),
(8,		'Novitzk',			'parkssoj6732@gmail.com',		'gkfndpaks!2323', 0),
(9,		'notMyThing',		'parksj123@gmail.com',			'rudoll0222@', 1),
(10,	'goodtime_fornew',	'jsh07234@naver.com',			'wjdtlgus07234!', 1);
--(11,	'new_user',			'test',							'test',			0);
GO
---- BRAND

PRINT 'LEVEL 2 : BRAND';
INSERT INTO BRAND(BRAND_ID, BRAND_NAME, FOUNDATION_YEAR) VALUES
(0, 'NO OR DELETED', 0000),
(1, 'Maison Margiela', 1988),
(2, 'Raf Simons', 1995),
(3, 'Rick Owens', 1994),
(4, 'Helmut Lang', 1986),
(5, 'Comme des Garcons', 1969),
(6, 'Issey Miyake', 1970),
(7, 'Nike', 1964);
GO

---- APPRAISER
PRINT 'LEVEL 3 : APPRAISER'
INSERT INTO APPRAISER(APPRAISER_ID, APPI_NAME, LICENSE_NO, SPECIALTY, CAREER_YEARS, STATUS, RELIABILITY) VALUES
(0, '퇴사 처리 DUMMY', 0000000, 0, 0, 'INACTIVE', 0),
(1, '김전문', 1234434, 1, 12, 'ACTIVE', 95),		
(2, '이시몬', 2345545, 2, 15, 'ACTIVE', 98),		
(3, '박고딕', 3456656, 3, 8, 'ACTIVE', 90),		
(4, '최미니', 4567767, 4, 14, 'ACTIVE', 94),		
(5, '정가르송', 5678878, 5, 11, 'ACTIVE', 92),	
(6, '강플리츠', 6789989, 6, 10, 'ACTIVE', 91),
(7, '김일권', 9782431, 7, 10, 'ACTIVE', 91);	
GO

---- ITEM
PRINT 'LEVEL 4 : ITEM';
INSERT INTO ITEM (ITEM_ID, MODEL_NAMES, POST_TITLE, SEASON, ITEM_SIZE, USER_CONDITIONS, PRICE, BUY_STATUS, DESIGNER, BRAND_ID, OWN_USER, ORIGINAL_MODEL) VALUES
-- =============================================
-- ADMIN 보호용 레퍼런스 아이템 (OWN_USER=1, BUY_STATUS='숨김')
-- 복각 제품들이 참조하는 원형 모델 보호 목적
-- =============================================
(100, 'Tabi Split-Toe Boots 97SS',        '[REF] 마르지엘라 타비 부츠 97SS 원형',      '97SS', '270', '사용감 없음', 1, '숨김', 'Martin Margiela', 1, 1, NULL),
(101, 'Riot Riot Riot Bomber 01AW',        '[REF] 라프 시몬스 라이엇 봄버 01AW 원형',   '01AW', 'M',   '사용감 없음', 1, '숨김', 'Raf Simons',      2, 1, NULL),
(102, 'Helmut Lang Bondage Pants 98AW',    '[REF] 헬무트랭 본디지 팬츠 98AW 원형',      '98AW', 'M',   '사용감 없음', 1, '숨김', 'Helmut Lang',     4, 1, NULL),
(103, 'Pleats Please Wide Pants 95SS',     '[REF] 이세이미야케 플리츠 팬츠 95SS 원형',  '95SS', 'M',   '사용감 없음', 1, '숨김', 'Issey Miyake',    6, 1, NULL),
(104, 'Air Force 1 Low 82',                '[REF] 나이키 에어포스 1 로우 1982 원형',    '82SS', '270', '사용감 없음', 1, '숨김', 'Bruce Kilgore',   7, 1, NULL), -- [추가] Nike 원형 레퍼런스

-- =============================================
-- 일반 유저 등록 아이템
-- =============================================
-- Maison Margiela (1)
(1001, 'Tabi Split-Toe Boots',              '대박) 97SS 오리지널 타비부츠 상태 극상 풀박 팝니다', '97SS', '270', '사용감 없음', 2800000, '판매완료', 'Martin Margiela', 1, 2, NULL),
(1002, 'Tabi Split-Toe Boots Replica',      '마르지엘라 04FW 복각 타비부츠 (형들 상태 꿀매함)',   '04FW', '265', '약간의 오염',  950000, '판매완료', 'Martin Margiela', 1, 3, 100),
(1021, 'Maison Margiela 5AC Bag',           '정품) 메종마르지엘라 5AC 새들백 미시착 새상품급',     '21SS', 'ONE', '새상품',      3200000, '판매완료', 'John Galliano',   1, 7, NULL),

-- Raf Simons (2)
(1003, 'Riot Riot Riot Bomber',             '국내유일) 라프시몬스 01AW 라이엇 봄버 오리지널 대장급', '01AW', 'M',   '약간의 오염', 4500000, '판매완료', 'Raf Simons',      2, 4, NULL),
(1004, 'Riot Bomber Reissue',               '라프시몬스 18SS 라이엇봄버 리이슈 복각판 깔끔해요',    '18SS', 'S',   '사용감 없음', 1200000, '판매중',   'Raf Simons',      2, 5, 101),
(1014, 'Raf Simons Straight Denim',         '급처) 02FW 라프시몬스 스트레이트 생지 데님 팝니다',    '02FW', '31',  '약간의 오염',  780000, '판매중',   'Raf Simons',      2, 8, NULL),

-- Rick Owens (3)
(1005, 'DRKSHDW Drokkar High Top',          '크림택발) 릭오웬스 다크쉐도우 하이탑 영수증포함 새거',  '20FW', '43',  '새상품',       890000, '판매완료', 'Rick Owens',      3, 2, NULL),
(1006, 'Rick Owens Stooges Biker Jacket',   '가격내림) 릭오웬스 스투지 가죽 바이커 자켓 15AW',     '15AW', 'S',   '사용감 없음', 3100000, '판매완료', 'Rick Owens',      3, 6, NULL),
(1015, 'Rick Owens Moto Boots',             '릭오웬스 모토부츠 블랙 황금사이즈 상태좋음',          '18FW', '41',  '사용감 없음', 1450000, '판매완료', 'Rick Owens',      3, 2, NULL),

-- Helmut Lang (4)
(1007, 'Helmut Lang Bondage Pants',         '희귀명작) 98AW 오리지널 헬무트랭 본디지 팬츠 카키',    '98AW', 'S',   '약간의 오염', 2100000, '판매완료', 'Helmut Lang',     4, 3, NULL),
(1008, 'Helmut Lang Bondage Pants Re',      '헬무트랭 04SS 본디지 바지 복각버전 꿀매해가세요',      '04SS', 'M',   '사용감 없음',  680000, '판매중',   'Helmut Lang',     4, 7, 102),
(1016, 'Helmut Lang Latex Shirt 99AW',      '미친소재) 99AW 헬무트랭 라텍스 코팅 셔츠 팝니다',       '99AW', 'S',   '약간의 오염', 1750000, '판매중',   'Helmut Lang',     4, 5, NULL),

-- Comme des Garcons (5)
(1009, 'CDG Homme Plus Stripe Shirt',       'Comme) 02AW 꼼데가르송 옴므플러스 스트라이프 셔츠',   '02AW', 'M',   '사용감 없음', 1350000, '판매완료', 'Rei Kawakubo',    5, 4, NULL),
(1010, 'CDG Heart Logo Hoodie',             '매장판) 꼼데가르송 블랙 와펜 하트 후드 정품',          '23FW', 'L',   '새상품',       320000, '판매중',   'Rei Kawakubo',    5, 8, NULL),
(1017, 'CDG Homme Destroyed Knit 02AW',     '명작) 02AW 꼼데가르송 옴므 디스트로이드 보로니트',     '02AW', 'M',   '약간의 오염',  980000, '판매중',   'Rei Kawakubo',    5, 6, NULL),

-- Issey Miyake (6)
(1011, 'Pleats Please Wide Pants',          '이세이미야케 플리츠플리즈 와이드 팬츠 (원조 95SS)',   '95SS', 'M',   '사용감 없음',  980000, '판매완료', 'Issey Miyake',    6, 5, NULL),
(1012, 'Pleats Please Pants Replica',       '플리츠 플리즈 10SS 복각버전 가성비로 던집니다',        '10SS', 'S',   '약간의 오염',  420000, '판매중',   'Issey Miyake',    6, 6, 103),
(1018, 'Issey Miyake Bone Coat 94AW',       '초희귀) 94AW 이세이미야케 본 코트 아카이브 피스',      '94AW', 'M',   '사용감 없음', 4200000, '판매중',   'Issey Miyake',    6, 7, NULL),

-- Nike (7)
(1013, 'Nike Air Force 1 Low 82 OG',        '수집가용) 82년 오리지널 초판 나이키 에어포스 1 로우',  '82SS', '270', '약간의 오염', 5500000, '판매완료', 'Bruce Kilgore',   7, 2, NULL),
(1019, 'Nike Air Force 1 Reissue',          '실착강추) 나이키 에어포스 1 82년 원형 복각판 팝니다',  '22FW', '270', '사용감 없음',  180000, '판매중',   'Bruce Kilgore',   7, 3, 104),
(1020, 'Nike Air Air Jordan 1 Chicago 85',  '지존) 85년산 오리지널 에어조던 1 시카고 (소장급)',     '85AW', '265', '약간의 오염', 9900000, '판매중',   'Peter Moore',     7, 4, NULL);
GO

---- APPRAISAL
PRINT 'LEVEL 5 : APPRAISAL';
INSERT INTO APPRAISAL (ITEM_ID, APPRAISAL_NO, VERIFIED_GRADE, FINAL_RESULT, APPRAISER_ID, COMMENT) VALUES
-- 정상 통과
(1001, 1, 'S',  'PASS', 1, '97SS 타비 부츠 정품 확인. 족형 재봉선 및 각인 정상.'),
(1002, 1, 'A',  'PASS', 1, '복각 제품이나 상태 양호. 오리지널과 구분 명기됨.'),
(1003, 1, 'A+', 'PASS', 2, '01AW 라이엇 봄버 정품. 내부 태그 및 그래픽 일치.'),
(1005, 1, 'S',  'PASS', 3, '드로이디 하이탑 미착용 정품. 박스 및 더스트백 포함.'),
(1006, 1, 'A+', 'PASS', 3, '스투지 바이커 자켓 정품. 지퍼 각인 및 가죽 질감 확인.'),
(1009, 1, 'A',  'PASS', 5, 'CDG 옴므 플러스 02AW 정품. 내부 라벨 정상.'),
(1011, 1, 'A+', 'PASS', 6, '95SS 플리츠 플리즈 정품. 가공 처리 및 소재 확인 완료.'),
(1015, 1, 'S',  'PASS', 3, '릭 오웬스 모토 부츠 정품. 창 각인 및 봉제 상태 완벽.'),
(1013, 1, 'S',  'PASS', 7, '82년산 에어포스 초판 정품 확인. 중창 변색 외 가죽 무결.'),
(1019, 1, 'A',  'PASS', 7, '82년 복각판 제품. 자수 상태 및 패턴 오리지널과 대조 완료.'),
-- 불합격 후 재검수 케이스
(1007, 1, 'F',  'FAIL', 4, '본디지 팬츠 태그 위치 불일치. 재검수 권고.'),
(1007, 2, 'B',  'PASS', 4, '재검수 결과 정품 판정. 태그는 수선 이력으로 확인됨.'),
(1021, 1, 'F',  'FAIL', 1, '5AC 백 하드웨어 각인 미확인. 추가 서류 요청.'),
(1021, 2, 'B',  'PASS', 1, '구매 영수증 및 보증서 제출 후 정품 확인 완료.');
GO

---- TRANSACTIONS
PRINT 'LEVEL 6 : TRANSACTIONS';
INSERT INTO TRANSACTIONS (TRADE_ID, PRICE, PAYMENT_METHOD, TRANSACTION_DATE, ITEM_ID, SELLER_ID, BUYER_ID) VALUES
(1,  2800000, '신용카드',   '2026-03-15 14:22:00', 1001, 2, 4),
(2,  4500000, '계좌이체',   '2026-04-02 10:05:00', 1003, 4, 3),
(3,  890000,  '카카오페이', '2026-05-18 16:40:00', 1005, 2, 6),
(4,  3100000, '신용카드',   '2026-06-07 11:30:00', 1006, 6, 7),
(5,  2100000, '토스페이',   '2026-07-22 09:15:00', 1007, 3, 5),
(6,  1350000, '계좌이체',   '2026-08-11 13:50:00', 1009, 4, 2),
(7,  980000,  '네이버페이', '2026-09-03 17:25:00', 1011, 5, 8),
(8,  1450000, '신용카드',   '2026-10-19 15:10:00', 1015, 2, 4),
(9,  3200000, '카카오페이', '2026-11-05 12:00:00', 1021, 7, 3),
(10, 950000,  '토스페이',   '2026-12-01 18:45:00', 1002, 3, 5),
(11, 5500000, '계좌이체',   '2026-05-20 18:00:00', 1013, 2, 8);
GO