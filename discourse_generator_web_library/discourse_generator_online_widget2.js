/* LICENSED UNDER 0BSD rubygems.org/gems/democracy */        const ask =      (function(a=false,n=false,q=false,r=false,t=false,el=false,msg=false,NUM=false,num=false,ret=false,top=false,list=false,text=false,delay=false,list_=false,text2=false,utter=false,config=false,mutate=false,result=false,timing=false,prepend=false,started=false,timeout=false,duration=false,__inputs=false,new_list=false,new_text=false,observer=false,textArea=false,clicked_X=false,setup_lib=false,join_delim=false,successful=false,testSpeech=false,recognition=false,SpeechRecognition=false){ function debug() { console.log({a:a,n:n,q:q,r:r,t:t,el:el,msg:msg,NUM:NUM,num:num,ret:ret,top:top,list:list,text:text,delay:delay,list_:list_,text2:text2,utter:utter,config:config,mutate:mutate,result:result,timing:timing,prepend:prepend,started:started,timeout:timeout,duration:duration,__inputs:__inputs,new_list:new_list,new_text:new_text,observer:observer,textArea:textArea,clicked_X:clicked_X,setup_lib:setup_lib,join_delim:join_delim,successful:successful,testSpeech:testSpeech,recognition:recognition,SpeechRecognition:SpeechRecognition}) } 


  try {
  window["get_democracy_bar_input"] = f;

  function f() {
    if          ((!__inputs || !__inputs.length     ))
      __inputs = "sensibleness input".e().$reverse();
//    if (__inputs.length == 2 || __inputs.length == 1) {
  //    __inputs.$pop();
    //  return "";
//    }
    return __inputs.length ? __inputs.$pop().value : "";  
 } } catch(e) {
  }
  const ask = (end) => {
    setup_window();
    q = list.length;
    num = 0;
    t = "";
    t = "sensibleness input".e()
      .map((a) => {
        if (a.value.trim().length) {
          num++;
          /*a.style.color = "lightblue";
a.style.fontSize = "40px";
a.style.terSpacing = "10px";
a.style.textShadow = "5px 5px 5px black";*/
        } else if (q == list.length) {
          q = num;
        }
        return a.value;
      })
      .join(" ");
    if (q != NUM) t = list[num];
 //   speak = functi

    speak = (t, end, utter)   => {
    	if (t.trim().length == 0) {
    		window.speechSynthesis.speak(new SpeechSynthesisUtterance("0 length string"))
    		}
    		console.log(utter)
      try {
        utter = new SpeechSynthesisUtterance(t);
        utter.pitch = 1;
        utter.rate = 1;
      } catch (e) {
      	console.log("end");
        end();
        return;
      }
      utter.onend = end;
      if (window.speechSynthesis) {
      	function i() {}
        for (i.i = 0; i.i < 50; ++i.i) {
          speechSynthesis.cancel();
        }
        clear__();
//        recognition.stop();
        speechSynthesis.speak(utter);
      }
    };
    console.log("Speaking", t);

    if (q < NUM) {
            console.log(`${q} < ${NUM}`);
            alert(ack());
            alert(t);
      speak(ack(), () => speak(t, testSpeech));
    } else {
          console.log(`${q} < ${NUM}`);
      clearTimeout(timeout);
      recognition.stop();

      Opal.MainClass.MainClass.$new();
      speak("Copied to clipboard. To paste, right click", () => {});
//      $("sensibleness").map(


      "sensibleness".e().map(func);

//      function func() {

      function        func(i) {
      	i.innerHTML = ''
      	i.style.opacity = "0.0";
      }

      setup_lib = false;
    }
  };
  function not_filled(a) {
    new_text = a.value.replace(/[^\sa-zA-Z0-9]/g, "").replace(/\s\s/, "");
    new_text =
      new_text.charAt(0).toUpperCase() + new_text.substring(1, new_text.length);
    a.value = new_text;
    return a.value.length == 0;
  }
  const ack = () => {
    const list_ = result.split(/\s+/).$uniq();
    const prepend = list_.length < 6 ? result : "";
    const list = ["Thank you.", "Alright.", "Good.", "Very well."];
    if (list.length == 1) {
      list.push("Simple.");
    }
    const ret = prepend + list[Math.floor(Math.random() * list.length)];
    console.warn("Acking" + ret);
    return ret;
  };

  list = [
    "What is your negative feeling?",
    "What is your negative reason?",
    "What is your negative evidence?",
    "What is your positive feeling?",
    "What is your positive reason?",
    "What is your positive evidence?",
    "What is your neutral feeling?",
  ];

  try {
    if (!SpeechRecognition) SpeechRecognition = webkitSpeechRecognition;
  } catch (e) {}
  try {
    if (!SpeechRecognition)
      SpeechRecognition = class {
        start() {}
        stop() {}
      };
  } catch (e) {}

  recognition = new SpeechRecognition();
  more();

  function setup_window() {
    clicked_X = false;
    new_list = "sensibleness".e();
    //  alert(new_list)
    //alert([..._list])
    new_list.forEach((i) => {
      i.style.opacity = "1";
    });
    if (setup_lib) return;
    for (const program_div of new_list) {
      el = list;
      //      	alert(5)

      text2 = text =
        "<form action='javascript:document.activeElement.blur();ask();'><div style=\"float: left\">                              ";
      for (a = 0; a < NUM; ++a) {
        text += `<input placeholder="${el[a]}"  title="${el[a]}" value="" width="100%" id="input${a}" /><br/>`;
      }
      text += "</div>";
      text += text2;
      for (a = 0; a < NUM; ++a) {}
      text += "</div></form>";
      //alert(i)
      program_div.innerHTML = `
<div id="ct">
<!--<div style="display: inline-block; float: left" id="close_button"  onclick="[...arguments][0].target.parentNode.parentNode.style.opacity     = '0'     ;" title="Close">X</div>-->
<div style="float-left">${text}</div>
<div id="help">
Sensible, modern, democratic
</div>
<div id="help" class="foo">
Tell me about this
</div>
</div>`;
      //      );
    }
 //   setup_lib = true;

   // setu
    ".foo".$().addEventListener("click", click);

    function click() {
    	speak("Donate at gregorycohen2@gmail.com");
    }
    setup_lib = true;
  }
  function more() {
    NUM = list.length;
    result = " ";

    testSpeech = () => {
      recognition = new SpeechRecognition();
      recognition.interimResults = false;
      recognition.lang = "en-US";
      recognition.continuous = false;
      recognition.maxAlternatives = 1;
      recognition.onend = (event) => {
        for (i of "sensibleness input".e()) {
          if (not_filled(i)) {
            i.value = result;
            break;
          }
        }
        console.log(recognition)
        ask();
        result = "";
      };
      recognition.onspeechend = (event) => {
        clear__();
        event.preventDefault();
        recognition.stop();
      };
      recognition.onerror = (event) => {
        clear__();
        recognition.stop();
      };
      recognition.onaudiostart = (event) => {
        clear__();
      };
      recognition.onaudioend = (event) => {
        clear__();
        recognition.stop();
      };
      recognition.onresult = (event) => {
        const results = event.results;
        clear__();
        recognition.stop();

        r = results[results.length - 1][0].transcript;

        result = r[0].toUpperCase() + r.substr(1, r.length);
      };
      recognition.onnomatch = (event) => {
        clear__();
        recognition.stop();
      };
      recognition.onsoundstart = (event) => {
        clear__();
      };
      recognition.onsoundend = (event) => {
        clear__();
        recognition.stop();
      };
      recognition.onspeechstart = (event) => {
        clear__();
      };
      recognition.onstart = (event) => {
        clear__();
      };
      started = true;
      recognition.start();
    };
  }
  function clear__() {
    clearTimeout(timeout);
    timeout = undefined;
    add_timeout(timeout);
  }
  function add_timeout() {
    clearTimeout(timeout);
    timeout = setTimeout(() => {
      started = false;
      recognition.stop();
    }, 30000);
  }

  function fallbackCopyTextToClipboard(text) {
    textArea = document.createElement("textarea");
    textArea.value = text;

    textArea.style.top = "0";
    textArea.style.left = "0";
    textArea.style.position = "fixed";
    document.body.appendChild(textArea);
    textArea.focus();
    textArea.select();
    try {
      successful = document.execCommand("copy");
      msg = successful ? "successful" : "unsuccessful";
    } catch (err) {}
    document.body.removeChild(textArea);
  }

  

  window.copyTextToClipboard = (text) =>      {
    if (!navigator.clipboard) {
      fallbackCopyTextToClipboard(text);
      return;
    }
    navigator.clipboard.writeText(text);
  }

  const document = (() => {
    try {
      return window.document;
    } catch (e) {
      const v = {};
      for (const i of "addEventListener setAttribute createElement appendChild".split(
        " "
      )) {
        v[i] = () => {
          return v;
        };
      }
      v.head = v;

      /*		v.head = v;
return v*/ return v;
    }
  })();

  add_opal();

  document.head.insertAdjacentHTML(
    "afterbegin",
    `
	<meta charset="utf-8"/>

	<style type="text/css">



sensibleness {
		position: fixed;
		top: 0px;
		left: 0px;
		right: 0px;
		width: 100%;
		display: block;
		z-index: 2000;
		
		text-align: center;
font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", "Roboto", "Oxygen", "Ubuntu", "Cantarell", "Fira Sans", "Droid Sans", "Helvetica Neue",sans-serif;
}
em {
color: #0000CC;
}


* {
//	box-sizing: border-box;
font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", "Roboto", "Oxygen", "Ubuntu", "Cantarell", "Fira Sans", "Droid Sans", "Helvetica Neue",sans-serif;

}
//	box-
${css([
  ["body > div", "width", [600, 500, 900]],
  ["*:not(sensibleness)", "border-radius", [50, 30, 100]],
  ["*:not(sensibleness *)", "padding", [10, 50, 30]],
    //["*","border", ["5px solid black", "10px solid green", "20px solid white"]], //, ["
  ["*", "letter-spacing", "0px 0px 5px".split(" ")],

  ["#ct", "width", [400, 500, 600]],
  //["*", "font-weight", ["normal", "bold", "bold"]],
  //["*", "font-size", ["100%", "200%", "200%"]],

  ["#ct", "height", [400, 500, 600]],
  ["*:not(sensibleness)", "background-color", ["white", "white", "white"]],
  //["sensibleness", "background-color", "transparent".repeat(3).split(" ")],
  ["*", "color", ["black", "black", "black"]],
  ["#content", "transform", ["rotate(-3deg)", "rotate(0deg)", "rotate(0deg)"]],
  [
    " sensibleness #ct",
    "transform",
    ["rotate(0deg)", "rotate(0deg)", "rotate(0deg)"],
  ],
  [
    "*:not(sensibleness)",
    "box-shadow",
    ["3px 3px 3px #222222", "3px 3px 60px gold", "0px 0px 200px black"],
  ],

  ["#content", "opacity", ["0", 1, 1]],
  ["a", "display", "inline-block"],
  //["*","font-weight", ["bold"]],
])}

sensibleness * {
border-bottom-left-radius: 30px;
border-bottom-right-radius: 30px;
display: inline-block;
text-align: center;
//border: 1px solid black;
}
sensiblessness > form > div    { margin-left: 20px; margin-right: 20px; }
sensiblessness > form > div    { margin-left: 20px; margin-right: 20px; }
sensibleness #help {
float: right;
padding: 30px;
font-size:11px;
font-style: italic;
display: inline-block;
}
ct {
padding: 30px;
}
</style>

`
  );

  document.addEventListener("keyup", (key) => {
    if (key.key == "Escape" || key.key.match(/F[0-9][0-9]?/)) {
      "sensibleness ".e().forEach((a) => {
        a.style.opacity = "0";
      });
    }
  });


  const cancel_all = () => {
    speechSynthesis.cancel();
    recognition.stop();
    clearTimeout(timeout);
    clicked_X = true;
  }; /*
  }*/

  String.prototype.$ = function () {
    return document.querySelector(this);
  };

  String.prototype.e = function (i) {
    const string = this;
    const bad_list = document.querySelectorAll(string);
    const new_list = [];

    for (i = 0; i < bad_list.length; ++i) {
      new_list.push(bad_list[i]);
    }

    return new_list;
  };

  {
  }

  function add_opal() {
    //  	document.head.insertAdjacentHTML("<script type='text/javascript' src

    document.head.insertAdjacentHTML(
      "afterBegin",
      "<script type='text/javascript' src='discourse_generator_online_widget1.js'></script>"
    );

    //  	window.addEventListener("DOMCon
    window.addEventListener("DOMContentLoaded", postfix);

    //  	window.a

    //  	post

    //postfix();
  }

  function postfix(
    mutate = true,
    observer = document.body,
    delay = "",
    duration = "0.5s",
    timing = "cubic-bezier(30, 30, 30, 30)",
    text = "",
    join_delim = " ",
    top = document.body,
    config = { attributes: true, childList: true, subtree: true }
  ) {
    function set_elems(i, iter, n) {
      n = window.getComputedStyle(top);
      for (iter = 0; iter < n.length; ++iter) {
        text += n[iter] + " ";
      } //console.log(n);
      for (i of "*".e()) {
        i.style.transitionProperty = text;
        i.style.transitionDuration = duration;
        if (timing.length) i.style.transitionTimingFunction = timing;
        i.style.transitionDelay = delay;
      }
      return text;
    }
    set_elems();
    function f(mutationsList, observer) {
      postfix(false);
    }

    if (mutate) new MutationObserver(f).observe(top, config);
  }
  //  }

function c(selector, v, value) {
  if (typeof value == "string") value = value.split(" ");

  "border-radius padding margin font-size width height"
    .split(" ")
    .includes(v) && (value = value.map(q));
  function q(a) {
    a = a.toString();
    if (a.endsWith("px")) {
      return a;
    } else {
      return `${a}px`;
    }
  }
  // + "px" }}
  //			if a.endsW
  //	}
  function string() {}
  string.string = "";
  for (const selector_ of typeof selector == Array
    ? selector
    : selector.split(",")) {
    const selector = selector_.toString().trim();
    string.string += `
${selector} { ${v}:  ${value[0] || value}; }
${selector}:hover { ${v}: ${value[1] || value[0] || value};}
${selector}:focus { ${v}: ${value[2] || value[1] || value[0] || value}; }
${selector}:active { ${v}: ${value[2] || value[1] || value[0] || value}; }
`;
  }
  return string.string;
}

function css(arg, text = "") {
  for (const a of arg) {
    text += c(...a);
  }
  return text;
}

function wrap(c, a, b) {
	return a + c + b;
}

function add_ps(a) {
	return `<p style="width: 600px" id='p${++add_ps.value}'>${a}</p>`;
}

window.addEventListener("DOMContentLoaded", split_input);

add_ps.value = 0;

function split_input() {
	"#content".$().innerHTML="#content".$().innerText.split(/new\s+?se.+?\s+?/i).map(add_ps);
}


function add_copyright(i) {
	i.insertAdjacentHTML("beforeend", i, "No copyrights");
}


return ask;

})();


function alert() {}