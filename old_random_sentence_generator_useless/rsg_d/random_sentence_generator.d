import std.stdio;import std.random;
import std.math;
import std.string; import std.algorithm.searching;
import core.stdc.ctype;
//import std.ctype;
/*
import std.algori
import std.algorithm.sear
*/

double random() {
	double b = uniform!"[]"(cast(double)0.0, cast(double)1.0);
	return b;
	
}

uint random(uint length) {
//	return cast(uint)floor(cast(double)rand() / cast(double)-1u * (length - 1.0000000000000001e-15));

		uint b = uniform!"[]"(0u,   length - 1);
	return b;
}

T choice(T)(T[] words) {
	assert(words.length <= -1u);
	return words[random(cast(uint)words.length)];
}

string adverbs[] = [
	"really",
	"truly",
	"absolutely",
	"completely",
	"utterly"
];

import adjectives : adjectives;
import nouns_and_pronouns : nouns_and_pronouns;

string verbs[] = [
	"is",
	"isn't",
	"is not",
	
	"wasn't",
	"was not",
	
	"went",
	"makes",
	"creates",
	
	"eats",
	"is eating",
	"will be eating",
	"had been eating",
	"was eating",
	"wants to eat",
	
	"works",
	"is working",
	"will be working",
	"had been working",
	"was working",
	"wants to work",
	
	"runs",
	"is running",
	"will be running",
	"had been running",
	"was running",
	"wants to run",
	
	"kills",
	"is killing",
	"will be killing",
	"had been killing",
	"was killing",
	"wants to kill",
	
	"plays",
	"is playing",
	"will be playing",
	"had been playing",
	"was playing",
	"wants to play",
	
	"jumps",
	"is jumping",
	"will be jumping",
	"had been jumping",
	"was jumping",
	"wants to jump",
	
	"teleports",
	"is teleporting",
	"will be teleporting",
	"had been teleporting",
	"was teleporting",
	"wants to teleport",
	
	"thinks",
	"is thinking",
	"will be thinking",
	"had been thinking",
	"was thinking",
	"wants to think",
	
	"falls",
	"is falling",
	"will be falling",
	"had been falling",
	"was falling",
	"wants to fall",
	
	"talks",
	"is talking",
	"will be talking",
	"had been talking",
	"was talking",
	"wants to talk",
	
	"sits",
	"is sitting",
	"will be sitting",
	"had been sitting",
	"was sitting",
	"wants to sit",
	
	"stands",
	"is standing",
	"will be standing",
	"had been standing",
	"was standing",
	"wants to stand",
	
	"goes crazy",
	"is going crazy",
	"will be going crazy",
	"had been going crazy",
	"was going crazy",
	"wants to go crazy",
	
	"communicates",
	"is communicating",
	"will be communicating",
	"had been communicating",
	"was communicating",
	"wants to communicate",
	
	"sings",
	"is singing",
	"will be singing",
	"had been singing",
	"was singing",
	"wants to sing",
	
	"sprints",
	"is sprinting",
	"will be sprinting",
	"had been sprinting",
	"was sprinting",
	"wants to sprint",
	
	"whistles",
	"is whistling",
	"will be whistling",
	"had been whistling",
	"was whistling",
	"wants to whistle",
	
	"yells",
	"is yelling",
	"will be yelling",
	"had been yelling",
	"was yelling",
	"wants to yell",
	
	"sleeps",
	"is sleeping",
	"will be sleeping",
	"had been sleeping",
	"was sleeping",
	"wants to sleep",
	
	"laughs",
	"is laughing",
	"will be laughing",
	"had been laughing",
	"was laughing",
	"wants to laugh",
	
	"giggles",
	"is giggling",
	"will be giggling",
	"had been giggling",
	"was giggling",
	"wants to giggle",
	
	"yawns",
	"is yawning",
	"will be yawning",
	"had been yawning",
	"was yawning",
	"wants to yawn",
	
	"wins",
	"is winning",
	"will be winning",
	"had been winning",
	"was winning",
	"wants to win",
	
	"hears",
	"is hearing",
	"will be hearing",
	"had been hearing",
	"was hearing",
	
	"sees",
	"is seeing",
	"will be seeing",
	"had been seeing",
	"was seeing",
	
	"dies",
	"is dying",
	"will be dying",
	"had been dying",
	"was dying",
	"wants to die",
	
	"grabs",
	"is grabbing",
	"will be grabbing",
	"had been grabbing",
	"was grabbing",
	"wants to grab",
	
	"raps",
	"is rapping",
	"will be rapping",
	"had been rapping",
	"was rapping",
	"wants to rap",
	
	"fails",
	"is failing",
	"will be failing",
	"had been failing",
	"was failing",
	"wants to fail",
	
	"writes",
	"is writing",
	"will be writing",
	"had been writing",
	"was writing",
	"wants to write",
	
	"crawls",
	"is crawling",
	"will be crawling",
	"had been crawling",
	"was crawling",
	"wants to crawl",
	
	"flies",
	"is flying",
	"will be flying",
	"had been flying",
	"was flying",
	"wants to fly",
	
	"fights",
	"is fighting",
	"will be fighting",
	"had been fighting",
	"was fighting",
	"wants to fight",
	
	"dances",
	"is dancing",
	"will be dancing",
	"had been dancing",
	"was dancing",
	"wants to dance",
	
	"cries",
	"is crying",
	"will be crying",
	"had been crying",
	"was crying",
	"wants to cry",
	
	"moves",
	"is moving",
	"will be moving",
	"had been moving",
	"was moving",
	"wants to move",
	
	"kicks",
	"is kicking",
	"will be kicking",
	"had been kicking",
	"was kicking",
	"wants to kick",
	
	"reads",
	"is reading",
	"will be reading",
	"had been reading",
	"was reading",
	"wants to read",
	
	"succeeds",
	"is succeeding",
	"will be succeeding",
	"had been succeeding",
	"was succeeding",
	"wants to succeed",
	
	"attacks",
	"is attacking",
	"will be attacking",
	"had been attacking",
	"was attacking",
	"wants to attack",
	
	"drives",
	"is driving",
	"will be driving",
	"had been driving",
	"was driving",
	"wants to drive",
	
	"drinks",
	"is drinking",
	"will be drinking",
	"had been drinking",
	"was drinking",
	"wants to drink",
	
	"walks",
	"is walking",
	"will be walking",
	"had been walking",
	"was walking",
	"wants to walk",
	
	"explodes",
	"is exploding",
	"will be exploding",
	"had been exploding",
	"was exploding",
	"wants to explode",
	
	"loves",
	"is loving",
	"will be loving",
	"had been loving",
	"was loving",
	"wants to love",
	
	"watches",
	"is watching",
	"will be watching",
	"had been watching",
	"was watching",
	"wants to watch"
];

string ending_adverbs[] = [
	"incredibly",
	"horribly",
	"well",
	"normally"
];

string countries[] = [
	"Russia",
	"North Korea",
	"South Korea",
	"Canada",
	"the US",
	"Pakistan",
	"Estonia",
	"Germany",
	"Nazi Germany",
	"Hungary",
	"Italy",
	"Greece",
	"France",
	"Spain",
	"Egypt",
	"Mexico",
	"Brazil",
	"Chile",
	"Japan",
	"China",
	"India",
];

string owned_locations[] = [
	"bed",
	"house",
	"basement",
	"shoe",
	"body",
	"laptop",
	"computer",
	"room",
	"couch",
	"umbrella",
	"car"
];

string objects_in_space[] = [
	"Mercury",
	"Venus",
	"Earth",
	"Mars",
	"Jupiter",
	"Saturn",
	"Uranus",
	"Neptune",
	"Pluto"
];

string other_locations[] = [
	"Hogwarts",
	"space",
	"outer space",
	"Heaven",
	"Hell",
	"the Titanic",
	"a shoe",
	"the backpack",
];

string prepositions[] = [
	"on",
	"in",
	"with",
	"within",
	"around",
	"near",
	"under",
	"beside",
	"on top of",
	"inside of",
	"outside of",
	"on the bottom of",
	"on the side of",
	"away from",
	"far away from",
	"somewhat far away from",
	"very far away from",
	"close to",
	"somewhat close to",
	"very close to",
	"near",
	"somewhat near",
];

bool is_plural(string word) {
	switch (word) {
		case "feet":
		case "children":
		case "people":
		case "Windows PCs":
		case "men":
		case "women":
		case "plates of spaghetti":
		case "bowls of soup":
			return true;
		
		case "grass":
		case "glass":
		case "Steve Jobs":
		case "thesaurus":
		case "Bill Gates":
		case "Santa Claus":
		case "Chuck Norris":
		case "Elvis":
		case "Starbucks":
			return false;
		
		default:
			return word[$ - 1] == 's';
	}
}

string pluralize_so_that_generalities_can_be_made(string word) {
	switch (word) {
		case "foot": return "feet";
		case "child": return "children";
		case "person": return "people";
		case "man": return "men";
		case "woman": return "women";
		case "thief": return "thieves";
		case "city": return "cities";
		case "ruby": return "rubies";
		case "plate of spaghetti": return "plates of spaghetti";
		case "bowl of soup": return "bowls of soup";
		case "chimney": return "chimnies";
		case "Mr. T": return "Mr. T's";
		case "PC": return "PC's";
		case "Windows PC": return "Windows PC's";
		default:
			if (word[$ - 1] == 's' || word[$ - 1] == 'x' || word[$ - 1] == 'z') {
				return word ~ "es";
			}
			
			return word ~ "s";
	}
}

string sometimes_get_a_clause_sometimes_get_an_empty_string(double chance) {
	string preposition_object = choice(nouns_and_pronouns);
	
	if (random() < chance) {
		string beginning_of_clause;
		
		beginning_of_clause = choice([
			"because",
			"since"
		]);
		
		string state_of_being_verb;
		bool proper_noun;
		
		if (is_proper_noun(preposition_object)) {
			state_of_being_verb = "is";
			proper_noun = true;
		} else {
			state_of_being_verb = "are";
			proper_noun = false;
			
			if (!preposition_object.is_plural()) {
				preposition_object = pluralize_so_that_generalities_can_be_made(preposition_object);
			}
		}
		
		string clause = beginning_of_clause ~ " ";
		
		double number = random();
		
		if (number < (1.0 / 4.0)) {
			if (!proper_noun && state_of_being_verb == "are") {
				clause ~= "all ";
			}
			
			clause ~= preposition_object;
			clause ~= " " ~ state_of_being_verb ~ " " ~ choice(adjectives);
		} else if (number < (2.0 / 4.0)) {
			clause ~= preposition_object;
			clause ~= " " ~ state_of_being_verb ~ " sometimes " ~ choice(adjectives);
		} else if (number < (3.0 / 4.0)) {
			clause ~= preposition_object;
			clause ~= " " ~ state_of_being_verb ~ " rarely " ~ choice(adjectives);
		} else {
			clause ~= preposition_object;
			clause ~= " " ~ state_of_being_verb ~ " never " ~ choice(adjectives);
		}
		
		return clause;
	}
	
	return "";
}

bool is_vowel(char char_) {
	switch (char_) {
		case 'a':
		case 'e':
		case 'i':
		case 'o':
		case 'u':
		case 'A':
		case 'E':
		case 'I':
		case 'O':
		case 'U':
			return true;
		default:
			return false;
	}
}

bool starts_with_vowel(string str) {
	return is_vowel(str[0]);
}

bool should_be_prefixed_with_an_article(string word) {
	switch (word) {
		case "McDonald's restaurant":
		case "Blu-ray disk":
		case "BlackBerry":
		case "PC":
		case "Windows PC":
		case "MacBook Pro":
		case "CD":
		case "DVD":
		case "Mac":
		case "Macintosh":
		case "MacBook":
		case "iPod":
		case "iPod Touch":
		case "Pope":
		case "Internet":
		case "New Yorker":
		case "Dr. Pepper":
		case "Easter Bunny":
		case "Coke machine":
		case "Pepsi machine":
		case "Ubuntu machine":
		case "Windows computer":
		case "Christmas carol":
			return true;
		case "God":
			if (random() < 0.2) {
				return true;
			} else {
				return false;
			}
		default:
			return false;
	}
}

bool startswith(string str1, string str2) {
	return str1.length >= str2.length && str1[0 .. str2.length] == str2;
}

bool endswith(string str1, string str2) {
	return str1.length >= str2.length && str1[$ - str2.length .. $] == str2;
}

string add_article_to_noun_or_pronoun(string word) {
	if (word.is_plural() || word == "grass" || word == "chicken soup" || random() < 0.5) {
		return "the " ~ word;
	} else {
		if (word.starts_with_vowel()) {
			return "an " ~ word;
		} else {
			return "a " ~ word;
		}
	}
}

string add_article_to_noun_or_pronoun_if_necessary(string word) {
	string last_word = word.split()[$ - 1];
	
	if (word.is_plural()) {
		return add_article_to_noun_or_pronoun(word);
	}
	
	if (isupper(last_word[0]) && !word.should_be_prefixed_with_an_article()) {
		return word;
	}
	
	return add_article_to_noun_or_pronoun(word);
}

string get_random_preposition_object() {
	return add_article_to_noun_or_pronoun_if_necessary(choice(nouns_and_pronouns));
}

string[] get_adverbs_in_sentence() {
	string[] adverbs_in_sentence = [];
	
	if (random() < (1.0 / 6.0)) {
		adverbs_in_sentence ~= choice(adverbs);
	
		if (random() < (1.0 / 7.0)) {
			adverbs_in_sentence ~= choice(adverbs);
		
			if (random() < (1.0 / 12.0)) {
				adverbs_in_sentence ~= choice(adverbs);
			}
		}
	}
	
	return adverbs_in_sentence;
}

bool is_proper_noun(string noun_or_pronoun) {
	return
		!is_plural(noun_or_pronoun) &&
		isupper(noun_or_pronoun[0]) &&
		!noun_or_pronoun.should_be_prefixed_with_an_article();
}

string get_third_person_singular(string noun_or_pronoun) {
	if (noun_or_pronoun.is_plural()) {
		return "them";
	}
	
	return "it";
}

void get_question(string[] nouns_and_pronouns, string noun_or_pronoun, ref string[] sentence) {
	string verb;
	
	bool should_begin_sentence_with_why = random() < 0.5;
	bool do_or_does = false;
	
	if (should_begin_sentence_with_why) {
		sentence ~= "Why";
		
		if (random() < 0.5) {
			do_or_does = true;
			
			if (random() < 0.5) {
				sentence ~= [is_plural(noun_or_pronoun) ? "do" : "does"];
			} else {
				sentence ~= "did";
			}
		} else {
			sentence ~= [is_plural(noun_or_pronoun) ? "are" : "is"];
		}
	} else {
		sentence ~= [is_plural(noun_or_pronoun) ? "Are" : "Is"];
	}
	
	if (do_or_does) {
		do {
			verb = choice(verbs);
		} while (!verb.startswith("wants to "));
		
		verb = verb[9 .. $];
		
		if (verb == "explode") {
			sentence[$ - 1] = "did";
		}
		
		if (verb != "sing" && verb.endswith("ing")) {
			verb = verb[0 .. $ - 3];
		}
	} else {
		do {
			verb = choice(verbs);
		} while (!verb.startswith("is "));

		verb = verb[3 .. $];
	}
	
	sentence ~= add_article_to_noun_or_pronoun_if_necessary(noun_or_pronoun);
	sentence ~= verb;
	
	if (verb.find("watch") != "") {
		sentence ~= get_random_preposition_object();
	}
	
	sentence[$ - 1] ~= "?";
	
	if (random() < (1.0 / 3.0)) {
		sentence[$ - 1] ~= '!';
	}
	
	if (random() < 0.15) {
		if (should_begin_sentence_with_why) {
			if (random() < 0.5) {
				sentence ~= "It's";
				
				if (random() < (2.0 / 3.0)) {
					sentence ~= "very";
				}
				
				sentence ~= "strange.";
			} else {
				string noun_or_pronoun2;
				
				do {
					noun_or_pronoun2 = choice(nouns_and_pronouns);
				} while (noun_or_pronoun2 == noun_or_pronoun);
				
				string third_person_singular = get_third_person_singular(noun_or_pronoun);
				noun_or_pronoun2 = add_article_to_noun_or_pronoun_if_necessary(noun_or_pronoun2);
				
				sentence ~= ["Because", noun_or_pronoun2, "told", third_person_singular, "to."];
			}
		} else {
			sentence ~= ["I", "hope", choice(["so", "not"]) ~ choice(".!")];
		}
	}
}

string get_article(string noun_or_pronoun, bool adjectives_in_sentence, string[] adverbs_in_sentence, string adjective) {
	if (noun_or_pronoun.is_plural()) {
		return "The";
	} else {
		if (adjectives_in_sentence) {
			if (adverbs_in_sentence.length) {
				if (random() < 0.5) {
					return "The";
				} else {
					if (adverbs_in_sentence[0].starts_with_vowel()) {
						return "An";
					} else {
						return "A";
					}
				}
			} else {
				if (random() < 0.5) {
					return "The";
				} else {
					if (adjective.starts_with_vowel()) {
						return "An";
					} else {
						return "A";
					}
				}
			}
		} else {
			if (random() < 0.5) {
				return "The";
			} else {
				if (noun_or_pronoun.starts_with_vowel()) {
					return "An";
				} else {
					return "A";
				}
			}
		}
	}
}

string get_sentence(string[] names = []) {
	string adjective;
	bool adjectives_in_sentence = random() < 0.5;
	string[] sentence = [];
	string clause;
	string preposition;
	string country = "";
	bool tried_to_or_decided = false;
//	string preposition, cla
	
	enum : uint {PERIOD_CHOSEN, EXCLAMATION_POINT_CHOSEN, QUESTION_MARK_CHOSEN};
	uint punctuation_mark_chosen;
	
	if (names.length) {
		nouns_and_pronouns = names;
	}
	
	if (adjectives_in_sentence) {
		adjective = choice(adjectives);
	}

	string verb;
	string noun_or_pronoun = choice(nouns_and_pronouns);
	string[] adverbs_in_sentence = null;
	string article;
	bool proper_noun;
	
	if (random() < 0.2) {
		get_question(nouns_and_pronouns, noun_or_pronoun, sentence);
		punctuation_mark_chosen = QUESTION_MARK_CHOSEN;
		goto end;
	}
	
	proper_noun = is_proper_noun(noun_or_pronoun);
	
	if (adjectives_in_sentence) {
		adverbs_in_sentence = get_adverbs_in_sentence();
	}
	
	if (!proper_noun) {
		article = get_article(noun_or_pronoun, adjectives_in_sentence, adverbs_in_sentence, adjective);
	}
	
	verb = choice(verbs);
	
	if (noun_or_pronoun.is_plural()) {
		if (verb.endswith("goes crazy")) {
			verb = verb[0 .. $ - 10] ~ "go crazy";
		}
		
		verb = verb.replace("watches", "watch");
		
		if (verb == "wasn't") {
			verb = "weren't";
		} else if (verb == "was not") {
			verb = "were not";
		} else if (verb == "isn't") {
			verb = "aren't";
		} else if (verb == "is not") {
			verb = "are not";
		} else if (verb.startswith("is")) {
			verb = "are" ~ verb[2..$];
		} else if (verb.startswith("was")) {
			verb = "were" ~ verb[3..$];
		} else if (verb.startswith("wants")) {
			verb = "want" ~ verb[5..$];
		} else if (verb[$ - 1] == 's') {
			switch (verb) {
				case "flies":
					verb = "fly";
					break;
				case "cries":
					verb = "cry";
					break;
				default:
					verb = verb[0 .. $ - 1];
					break;
			}
		}
	}
	
	if (!proper_noun) {
		sentence ~= [article];
		
		if (adverbs_in_sentence.length && adjectives_in_sentence) {
			sentence ~= adverbs_in_sentence.join(", ");
			sentence[$ - 1] ~= ' ';
		}
		
		if (adjectives_in_sentence) {
			if (adverbs_in_sentence.length) {
				sentence[$ - 1] ~= adjective;
			} else {
				sentence ~= adjective;
			}
		}
		
		if (adjectives_in_sentence) {
			uint chosen_number = choice([0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2]);
			uint i = 0;
			
			while (i < chosen_number) {
				string new_adjective = choice(adjectives);
				
				sentence[$ - 1] ~= ",";
				sentence ~= new_adjective;
				
				++i;
			}
		}
	}
	
	preposition = choice(prepositions);
	
	clause = sometimes_get_a_clause_sometimes_get_an_empty_string(0.1);
	
	if (clause != "") {
		if (sentence.length) {
			sentence[0] =
				std.string.toUpper(clause[0..1]) ~ clause[1..$] ~ ", " ~
				cast(char)core.stdc.ctype.tolower(sentence[0][0]) ~ sentence[0][1..$];
		} else {
			sentence = [std.string.toUpper(clause[0..1]) ~ clause[1..$] ~ ","];
		}
	}
	
	
	
	if (random() < 0.15) {
		if (random() < 0.5) {
			if (verb.startswith("wants")) {
				verb = verb.replace("wants", "tried");
				tried_to_or_decided = true;
			}
		} else {
			if (verb.startswith("wants")) {
				verb = verb.replace("wants", "decided");
				tried_to_or_decided = true;
			}
		}
	}
	
	if (proper_noun && adjectives_in_sentence) {
		sentence ~= noun_or_pronoun;
		
		if (random() < 0.22) {
			sentence[$ - 1] ~= ", who was";
		} else {
			if (random() < 0.25) {
				sentence[$ - 1] ~= ", who isn't";
			} else {
				sentence[$ - 1] ~= ", who is";
			}
		}
		
		if (adverbs_in_sentence.length) {
			sentence ~= adverbs_in_sentence.join(", ");
		}
		
		sentence ~= [adjective ~ ",", verb];
	} else {
		sentence ~= [
			noun_or_pronoun,
			verb
		];
	}
	
	if (verb.find("watch") !=    "") {
		sentence ~= get_random_preposition_object();
	}
	
	switch (verb) {
		case "make":
		case "makes":
		case "create":
		case "creates":
			sentence ~= get_random_preposition_object();
			break;
		case "kill":
		case "kills":
			if (random() < 0.5) {
				sentence ~= get_random_preposition_object();
			}
			break;
		case "love":
		case "loves":
		case "grabs":
		case "grabbing":
			if (random() < 0.85) {
				sentence ~= get_random_preposition_object();
			}
		default:
	}
	
	if (
		sentence[$ - 1] != "is" &&
		sentence[$ - 1] != "isn't" &&
		sentence[$ - 1] != "is not" &&
		sentence[$ - 1] != "are" &&
		sentence[$ - 1] != "aren't" &&
		sentence[$ - 1] != "are not" &&
		sentence[$ - 1] != "wasn't" &&
		sentence[$ - 1] != "was not" &&
		sentence[$ - 1] != "weren't" &&
		sentence[$ - 1] != "were not" &&
		random() < 0.15
	) {
		sentence ~= choice(ending_adverbs);
	}
	
	if (random() < 0.5) {
		double number = random();
		
		if (number < (1.0 / 4.0)) {
			country = choice(countries);
			
			sentence ~= "in";
			sentence ~= country;
		} else if (number < (2.0 / 4.0)) {
			string owned_location = choice(owned_locations);
			
			string owner;
			
			do {
				owner = get_random_preposition_object();
			} while (owner.is_plural());
			
			sentence ~= "in " ~ owner ~ "'s " ~ owned_location;
		} else if (number < (3.0 / 4.0)) {
			sentence ~= "on";
			sentence ~= choice(objects_in_space);
		} else {
			sentence ~= "in";
			sentence ~= choice(other_locations);
		}
	}
	
	if (random() < (1.0 / 4.0)) {
		string verb2;
		
		do {
			verb2 = choice(verbs).split(" ")[$ - 1];
		} while (!verb2.endswith("ing"));
		
		sentence ~= "while " ~ verb2;
		
		if (verb2 == "attacking") {
			sentence ~= choice(["something", "someone"]);
		}
		
		if (random() < 0.12) {
			sentence ~= choice(ending_adverbs);
		}
	}
	
	if (random() < 0.5) {
		sentence ~= choice(prepositions);
		sentence ~= get_random_preposition_object();
	}
	
	if (tried_to_or_decided && clause == "") {
		sentence[$ - 1] ~= ", but failed";
		
		if (random() < (1.0 / 6.0)) {
			sentence ~= "horribly";
		}
	}
	
	if (clause == "" && !tried_to_or_decided) {
		clause = sometimes_get_a_clause_sometimes_get_an_empty_string(0.1);
		
		if (clause != "") {
			clause = ", " ~ clause;
		}
		
		sentence[$ - 1] ~= clause;
	}
	
	if (random() < 0.1) {
		sentence[$ - 1] ~= "!";
		punctuation_mark_chosen = EXCLAMATION_POINT_CHOSEN;
	} else {
		sentence[$ - 1] ~= ".";
		punctuation_mark_chosen = PERIOD_CHOSEN;
	}
	
	end:
	
	string joined_sentence = sentence.join(" ").replace("all things", "everything").replace("everything are", "everything is");
	
	if (country != "" && punctuation_mark_chosen == PERIOD_CHOSEN && random() < 0.1) {
		string word_meaning_therefore = choice([
			"Therefore",
			"Thus",
			"Consequently",
			"Hence",
			"As a result",
			"For that reason"
		]);
		
		joined_sentence ~= " ";
		
		if (country == "Nazi Germany") {
			country = "Germany";
		}
		
		if (joined_sentence.find("will be")      !=   "") {
			if (random() < 0.5) {
				joined_sentence ~= word_meaning_therefore ~ ", ";
				joined_sentence ~= "the entire country of " ~ country ~ " will become ";
			} else {
				joined_sentence ~= "The entire country of " ~ country ~ " will then become ";
			}
			
			joined_sentence ~= choice(adjectives) ~ ".";
		} else if (joined_sentence.find("is") != "" || joined_sentence.find("are") != "") {
			joined_sentence ~= word_meaning_therefore ~ ", the entire country of ";
			joined_sentence ~= country ~ " will soon become " ~ choice(adjectives) ~ ".";
		} else {
			if (random() < 0.5) {
				joined_sentence ~= word_meaning_therefore ~ ", ";
				joined_sentence ~= "the entire country of " ~ country ~ " became ";
			} else {
				joined_sentence ~= "The entire country of " ~ country ~ " then became ";
			}
			
			joined_sentence ~= choice(adjectives) ~ ".";
		}
	}
	
	return joined_sentence;
}

///*
void main() {
	while (true) {
		writefln(get_sentence());
	}
}
//*/
