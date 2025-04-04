#ifndef _ERROR_H
#define _ERROR_H

/**
	에러 코드를 정의

	@name 에러코드
 */
//@{
/**
	기타에러

	#define E_ERROR	-1
 */
#define E_ERROR		-1

/**
	잘못된 식별자

	#define E_BADID	-2
 */
#define E_BADID		-2

/**
	잘못된 파일 이름

	#define E_BADFILENAME	-3
 */
#define E_BADFILENAME		-3

/**
	잘못된 파일 위치

	#define E_BADSEEKPOS	-4
 */
#define E_BADSEEKPOS		-4

/**
	해당 리소스가 이미 존재함

	#define E_EXIST	-5
 */
#define E_EXIST		-5

/**
	잘못된 포맷

	#define E_BADFORMAT	-6
 */
#define E_BADFORMAT		-6

/**
	오퍼레이션 수행중

	#define E_INPROGRESS	-7
 */
#define E_INPROGRESS		-7

/**
	현재 사용중이거나 이미 사용중

	#define E_INUSE		-8
 */
#define E_INUSE			-8

/**
	매개변수가 잘못되었음

	#define E_INVALID	-9
 */
#define E_INVALID		-9

/**
	이미 연결이 설정되어 있음

	#define E_ISCONN	-10
 */
#define E_ISCONN		-10

/**
	제한길이 초과

	#define E_LONGNAME	-11
 */
#define E_LONGNAME	-11

/**
	내용 없음

	#define E_NOENT	-12
 */
#define E_NOENT		-12

/**
	남은 공간이 없음

	#define E_NOSPACE	-13
 */
#define E_NOSPACE		-13

/**
	연결이 설정되어 있지 않음

	#define E_NOTCONN	-14
 */
#define E_NOTCONN		-14

/**
	비어있지 않음

	#define E_NOTEMPTY -15
 */
#define E_NOTEMPTY	-15

/**
	해당 서비스를 지원하지 않음

	#define E_NOTSUP	-16
 */
#define E_NOTSUP		-16

/**
	메모리 부족

	#define E_NOMEMORY -17
 */
#define E_NOMEMORY	-17

/**
	버퍼가 작음

	#define E_SHORTBUF -18
 */
#define E_SHORTBUF	-18

/**
	would block 발생

	#define E_WOULDBLOCK	-19
 */
#define E_WOULDBLOCK	-19

/**
	타임아웃

	#define E_TIMEOUT	-20
 */
#define E_TIMEOUT		-20

/**
	데이터가 너무 큼

	#define E_DATABIG	-21
 */
#define E_DATABIG		-21

/**
	잘못된 레코드 식별자

	#define E_BADRECID -22
 */
#define E_BADRECID	-22

/**
	파일의 끝

	#define E_EOF		-23
 */
#define E_EOF			-23

/**
	접근에러

	#define E_ACCESS	-24
 */
#define E_ACCESS		-24

/**
	리소스 부족 

	#define E_NORESOURCES	-25
 */
#define E_NORESOURCES	(-25)

/**
	리소스 Free로 깨어남 
 */
#define E_FREERESOURCE	(-26)


//@}

#endif /* _ERROR_H */
