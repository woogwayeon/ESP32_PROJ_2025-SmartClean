document.addEventListener("DOMContentLoaded", function() {
    const form = document.querySelector("form");
    const pwInput = document.querySelector('input[name="user_pw"]');
    const pwCheckInput = document.querySelector('input[name="user_pw_ck"]');
    const pwBtn = document.querySelector('.pw input[type="button"]');

    let isPwConfirmed = false;

    function allowOnlyValidChars(event) {
        const key = event.key;
        const valid = /^[A-Za-z0-9!@#$%^&*]$/;
        if (!valid.test(key) &&
            key !== "Backspace" &&
            key !== "Delete" &&
            key !== "ArrowLeft" &&
            key !== "ArrowRight" &&
            key !== "Tab") {
            event.preventDefault();
        }
    }

    pwInput.addEventListener("keydown", allowOnlyValidChars);
    pwCheckInput.addEventListener("keydown", allowOnlyValidChars);

    pwBtn.addEventListener("click", function() {
        const pw = pwInput.value.trim();
        const pwCheck = pwCheckInput.value.trim();

        if (pw === "") {
            alert("비밀번호를 입력해주세요.");
            pwInput.focus();
            isPwConfirmed = false;
            return;
        }
        if (pw.length < 4) {
            alert("비밀번호는 최소 4자리 이상이어야 합니다.");
            pwInput.focus();
            isPwConfirmed = false;
            return;
        }
        if (pw.length > 26) {
            alert("비밀번호는 최대 26자리까지 입력할 수 있습니다.");
            pwInput.focus();
            isPwConfirmed = false;
            return;
        }
        if (pwCheck === "") {
            alert("비밀번호 확인란이 비어있습니다.");
            pwCheckInput.focus();
            isPwConfirmed = false;
            return;
        }
        if (pw !== pwCheck) {
            alert("비밀번호가 일치하지 않습니다.");
            pwCheckInput.value = "";
            pwCheckInput.focus();
            isPwConfirmed = false;
            return;
        }

        isPwConfirmed = true;
        alert("비밀번호가 일치합니다!");
    });

    submitBtn.addEventListener("click", function(e) {
        if (!isPwConfirmed) {
            alert("비밀번호 확인을 진행하지 않았습니다.");
            e.preventDefault();
        } else {
            // submit 전에 비밀번호 필드 초기화
            pwInput.value = "";
            pwCheckInput.value = "";
            // form은 그대로 제출됨 (GET 파라미터 포함)
        }
    });
});