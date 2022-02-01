var __inputs = undefined;
//var cancel_all = false;
get_democracy_bar_input = function() {
	if (!__inputs ||(__inputs.length&&__inputs.length==0))
		__inputs = get_inputs().$reverse();
	return (__inputs.length ? __inputs.$pop().value : "");
}

  get_inputs = () => {
    var new_list = [];
    var w = document.querySelectorAll("sensibleness input");
    for (i = 0; i < w.length; ++i) {
      new_list[i] = w[i];
    }

    return new_list;
  };

ask = function () {
  setup_window();

  var q = 0;
  var num = 0;
  var t = "";
  t = get_inputs()
    .map((a) => {
      if (a.value.trim().length) {
        num++;
        a.style.color = "lightblue";
        a.style.fontSize = "40px";
        a.style.letterSpacing = "10px";
        a.style.textShadow = "5px 5px 5px black";
      } else if (q == 0) {
        q = num;
//        console.log(q);
      }
      return a.value;
    })
    .join(" ");
  if (q != NUM) t = list[num];
  var utter = "";

  try {
    utter = new SpeechSynthesisUtterance(result ?(ack()+" " + t) : "");
  } catch (e) {}
  utter.pitch = 1;
  utter.rate = 1;

  if (q < NUM) {
    utter.onend = function () {
//      console.log("End");
      testSpeech();
    };

//    console.log(utter);
    if (window.speechSynthesis) {
      for (var i = 0; i < 50; ++i) {
        window.speechSynthesis.cancel();
      }

      clear__();
      recognition.stop();



      window.speechSynthesis.speak(utter);
      {}
    }
  } else {
    clearInterval(interval);
    recognition.stop();
  }
};
var clicked_X = false;

function not_filled(a) {
  var new_text = a.value.replace(/[^\sa-zA-Z0-9]/g, "").replace(/\s\s/, "");
  new_text =
    new_text.charAt(0).toUpperCase() + new_text.substring(1, new_text.length);
  a.value = new_text;
  return a.value.length == 0;
}
function ack() {
  var list = ["Thank you.", "Alright.", "Good.", "Very well."];
  return list[Math.floor(Math.random() * 4)];
}
window.list = [
"What is your negative feeling?",
"What is your negative reason?",
"What is your negative evidence?",
"What is your positive feeling?",
"What is your positive reason?",
"What is your positive evidence?",
"What is your neutral feeling?",
"What is your neutral reason?",
"What is your neutral evidence?",
];
//var result;
try {
  var SpeechRecognition = SpeechRecognition || webkitSpeechRecognition;
} catch (e) {
  SpeechRecognition = function () {
    var a = {start: ()=>{}, stop: ()=>{}};
    return a;
  };
}

var recognition = new SpeechRecognition();

function setup_window() {
	  clicked_X = false;
	  var _list = document.getElementsByTagName("sensibleness");
	  
  var _list = document.getElementsByTagName("sensibleness");
  var new_list = [];
  for (var i = 0; i < _list.length; ++i) {
    new_list[i] = _list[i];
    
  }
  new_list.forEach((i) => { i.style.opacity = "1"; });
  
  if (window.setup_lib) return;
  new_list.forEach((i) => {
    if (!i.getElementsByTagName("input").length)
      for (a = 0; a < NUM; ++a) {
        var l = document.createElement("label");
        l.innerText = list[a];
        i.appendChild(l);
        var e = document.createElement("input");
        e.style.width = "50%";
        e.value = "";
        i.appendChild(e);
        var e = document.createElement("br");
        e.value = "";
        i.appendChild(e);
      }

      var div=document.createElement("div");
//      div.id = "c
      div.id = "ct";
//      div.innerHTML 
/*
/&*
document.body.addEventListener(
		document.body.addEventListeng
		*/document.body.addEventListener("keyup", function(key) {
			switch (key.key) {
				case "Escape":
				document.querySelectorAll("#close_button").forEach(function(a) { a.click(); });
				document.body.click();
				break;
				case "Enter":
					document.activeElement.nextSibling.focus(); break; }
		});

      div.innerHTML = `
      	<div id="ct">
                                <img id="close_button" src="/home/a/Desktop/close.png" alt="X" onclick="var e = document.querySelector('sensibleness'); e.style.animationName = 'fade-in'; e.style.animationDuration =  '1.4s'; e.style.opacity = 0; cancel_all();"/>

                                <button onclick="Opal.MainClass.MainClass.$new()">Generate comment to clipboard</button>

                                <div id="help">
                                        Sensible, modern, democratic
                                </div></div>`;
//                        </div>`;       
      i.appendChild(div);
  });
  window.setup_lib = true;
}

var NUM = 9;
var result = " ";

function testSpeech() {
  recognition = new SpeechRecognition();
  recognition.interimResults = false;
  recognition.lang = "en-US";
  recognition.continuous = false;
  recognition.maxAlternatives = 1;

  recognition.onend = function (event) {
    //  speechResult = undefined;
 //   console.log("onend");
 //   console.log("RESULT" + result);
    // The SpeechRecognitionEvent results property returns a SpeechRecognitionResultList object
    // The SpeechRecognitionResultList object contains SpeechRecognitionResult objects.
    // It has a getter so it can be accessed like an array
    // The first [0] returns the SpeechRecognitionResult at position 0.
    // Each SpeechRecognitionResult object contains SpeechRecognitionAlternative objects that contain individual results.
    // These also have getters so they can be accessed like arrays.
    // The second [0] returns the SpeechRecognitionAlternative at position 0.
    // We then return the transcript property of the SpeechRecognitionAlternative object
    for (i of document.querySelectorAll("sensibleness input")) {
      if (not_filled(i)) {  i.value = result; break; }
    }
    if (clicked_X) {
//    	clicked_X = false;
    } else {
    	ask();
    }
    result = "";
  };


  recognition.onspeechend = function (event) {
    clear__();
    event.preventDefault();
    recognition.stop();
    //  	testSpeech();
    //    recognition.stop();
    //    testBtn.disabled = false;
    //  testBtn.textContent = 'Start new test';
  };

  recognition.onerror = function (event) {
    clear__();
    recognition.stop();
  };

  recognition.onaudiostart = function (event) {
    clear__();
    //Fired when the user agent has started to capture audio.
//    console.log("SpeechRecognition.onaudiostart");
  };

  recognition.onaudioend = function (event) {
    clear__();
    recognition.stop();
    //Fired when the user agent has finished capturing audio.
    console.log("SpeechRecognition.onaudioend");
  };

  recognition.onresult = function (event) {
    clear__();
    recognition.stop();
    result = event.results[event.results.length - 1][0].transcript;
    console.log(result);
    //Fired when the speech recognition service has disconnected.
    //      console.log('SpeechRecognition.onend');
  };

  recognition.onnomatch = function (event) {
    clear__();
    recognition.stop();
    //Fired when the speech recognition service returns a final result with no significant recognition. This may involve some degree of recognition, which doesn't meet or exceed the confidence threshold.
    console.log("SpeechRecognition.onnomatch");
  };

  recognition.onsoundstart = function (event) {
    clear__();
    //Fired when any sound — recognisable speech or not — has been detected.
  //  console.log("SpeechRecognition.onsoundstart");
  };

  recognition.onsoundend = function (event) {
    clear__();
    recognition.stop();
    //Fired when any sound — recognisable speech or not — has stopped being detected.
  //  console.log("SpeechRecognition.onsoundend");
  };

  recognition.onspeechstart = function (event) {
    clear__();
    //Fired when sound that is recognised by the speech recognition service as speech has been detected.
  //  console.log("SpeechRecognition.onspeechstart");
  };
  recognition.onstart = function (event) {
    clear__();
    //Fired when the speech recognition service has begun listening to incoming audio with intent to recognize grammars associated with the current SpeechRecognition.
//    console.log("SpeechRecognition.onstart");
  };
  started = true;
  recognition.start();
}



function clear__() {
  clearTimeout(timeout);
  timeout = undefined;
  add_timeout(timeout);
}

var timeout = undefined;
add_timeout = function () {
  clearTimeout(timeout);
  timeout = setTimeout(function () {
   // console.log("Timeout");
    started = false;
    recognition.stop();
  }, 2000);
};
//resu
function fallbackCopyTextToClipboard(text) {
  var textArea = document.createElement("textarea");
  textArea.value = text;
  
  // Avoid scrolling to bottom
  textArea.style.top = "0";
  textArea.style.left = "0";
  textArea.style.position = "fixed";

  document.body.appendChild(textArea);
  textArea.focus();
  textArea.select();

  try {
    var successful = document.execCommand('copy');
    var msg = successful ? 'successful' : 'unsuccessful';
    console.log('Fallback: Copying text command was ' + msg);
  } catch (err) {
    console.error('Fallback: Oops, unable to copy', err);
  }

  document.body.removeChild(textArea);
}
function copyTextToClipboard(text) {
  if (!navigator.clipboard) {
    fallbackCopyTextToClipboard(text);
    return;
  }
  navigator.clipboard.writeText(text).then(function() {
    console.log('Async: Copying to clipboard was successful!');
  }, function(err) {
    console.error('Async: Could not copy text: ', err);
  });
}

var new_css = document.createElement("style");
new_css.setAttribute("type", "text/css");
new_css.innerText = `
                @keyframes fade-in {
                    from {opacity: 0.5; transform: scale(1, 1) rotate(30deg); position: relative; left: 0px; top: 0px; }
                    to {opacity: 0; transform: scale(300, 300) rotate(30deg); position: relative; left: 0px; top: 600px; }
                }
                        body {
                                                display: inline-block;
                                                animation-timing-function: ease-in-out;
                                                text-align: center;

                                background-color: white;
//                              background-image: url("https://cdn.pixabay.com/photo/2019/11/20/20/02/couple-4641033_960_720.jpg");

font-family: -apple-system, BlinkMacSystemFont,
    “Segoe UI”, “Roboto”, “Oxygen”, “Ubuntu”, “Cantarell”,
    “Fira Sans”, “Droid Sans”, “Helvetica Neue”,
    sans-serif;
                        }

                        em {
                                color: #0000CC;
                        }

                        sensibleness input {
                                border-radius: 10px;
                                padding: 10px;
                             //   font-family: Times New Roman;
                                border: 1px solid white;
                                background-color: lightblue;
                                width: 300px;
                                /*text-align: center;*/
                        }
                        sensibleness {
//                              background-image: url("https://cdn.pixabay.com/photo/2013/12/17/20/10/bubbles-230014_960_720.jpg");
background: rgb(170,0,0);
background: -moz-linear-gradient(90deg, rgba(170,0,0,1) 0%, rgba(255,255,255,1) 100%);
background: -webkit-linear-gradient(90deg, rgba(170,0,0,1) 0%, rgba(255,255,255,1) 100%);
background: linear-gradient(90deg, rgba(170,0,0,1) 0%, rgba(255,255,255,1) 100%);
filter: progid:DXImageTransform.Microsoft.gradient(startColorstr="#aa0000",endColorstr="#ffffff",GradientType=1);
                                position: absolute;
                                z-index: 1000;
                                background-color: white;
                                top: 0px;
                                left: 0px;
                                box-shadow: 0px 0px 30px black;
                                border-bottom-left-radius: 30px;
                                border-bottom-right-radius: 30px;
                                width: 100%;
                                text-align: center;
                        }

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

                        button {
                                background-color:lightblue;
                                border-radius: 30px;
//                              posot
                                color: white;
                                text-shadow: 0px 0px 10px black;
                                box-shadow: 3px 3px 3px black;
                                position: absolute;
                                bottom: 0px;
                                border: 0px;
                        }

                        sensibleness img {
                                position: absolute;
                                top: 0px;
                                left: 0px;
                                padding: 30px;
                        }

`;

document.head.appendChild(new_css);


function cancel_all() {
	window.speechSynthesis.cancel();
	recognition.stop();
	clearTimeout(timeout);
	clicked_X = true;
}