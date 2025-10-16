function reg(){ location.href="#"; }
		
function check(){
			
			if(document.frm.inpId.value.length==0){
				alert("아이디를 입력해야 합니다");
				document.frm.inpId.focus();
				return false;
				// 리턴 false를 반환하면 그 이후의 동작 안됨 - 즉 제출안됨
				// 그래서 onclick을 쓸 때 return을 한번 더 잡아줌
			}
			if(document.frm.inpPw.value==""){
				alert("비밀번호를 입력해야 합니다");
				document.frm.inpPw.focus();
				return false;
			}
			
			return true;
			
}