/*

Copyright (C) 2014 Ole Lund Frederiksen


Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/


#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <ncurses.h>
#include <vector>
#include <string>
#include <sstream>
#include <math.h>
#include <time.h>
#include <iomanip>

#ifdef LINUX
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif
#ifdef WINDOWS
#inlcude <windows.h>
#endif

using namespace std;

/*
Game created in December/January 2013/2014 by Ole Lund Frederiksen.
* The game runs with the ncurses.h package and is written in C++
* There are two different modes in the game
* 1) with mathetical expressions
* 2) with words


* The user shall either type in the words written on the screen or the answer
* to the mathematical equations. If this is done before the words passes the screen 
* you will have get points, after 10 misses the game ends. 

* To DO
* -> Place it on Git, and the webpage to get critics.
* ->  
*/

class Word
{
	private:
	string word;  	// holding the word to type or mathematical expression to solve. 
	string result;  // holding the result to the mathematical expression.
	int row; 	 	// holding the row where the word is printed.
	int indent; 	// holding the indent of the word on the screen.
	
	public:
	Word(string& str, int num)
	{
            word=str;
            row=num;
            indent = 0;
	}

    Word(string& str, int num, int wordmath, string& resu) 
    {
            word=str;
            row=num;
            indent = 0;
            result = resu;			
    }	
    	
	string getword()
	{
		return word;
	}
	string getresult()
	{
		return result; 
	}
	
	int getrow()
	{
		return row;
	}
	
	int getindent()
	{
		return indent;		
	}
	
	int getlength()
	{
		return word.size();
	}

	int getvalue()
	{
		return (word.size()+10);
	}		
	
	void increaseIndent()
	{
		indent++;
	} 
}; 

class commandline
{
	private:
	string commandlineword; // the word written on the commandline in the buttom of the screen
	
	public:
	commandline()
	{
		commandlineword = "";
	};
	void clearcommandline()
	{
		commandlineword = "";
	}
	void removelastincommandline()
	{
		if (commandlineword.size() > 0)
		{
			commandlineword.erase (commandlineword.end()-1, commandlineword.end());		
		}
	}
	void addtocommandlineword(int chr)
	{
		commandlineword += chr;
	}
	string getcommandline()
	{
		return commandlineword;
	}	
};



class gameinfo
{
	private:
	struct timespec gamestarttime;  // starttime for the game.
	int gametype;			 	    // 0 for math game and 1 for word game. 
	int difficulty; 		 	    // determines the hardness of the (words, mathematical expressions).
	int score; 					    // holding the score for the game.
	int pressedkeys;			    // hold the number of keys pressed under the game.
	int correctkeys;		 	    // number of correct keys pressed under the game.
	int keystrokespersec; 	 	    // holds the number of keystrokes per second. 
	int wordmiss;  				    // hold the number of missed words.
	int maxnumberofwords;		    // maximum of simultaneous words on screen.
	int maxwordmiss; 			    // number of words misses before the game ends.
	int wronganswer;            	// the number of wrong math guesses.
	bool highscore;					// if the game shall be on the highscore board.
	double accuracy; 				// number to show the accuracy of the players typing.
			
	string rank;					 // the rank the player has achived.
	
	public:
	gameinfo(int type, int diff)
	{
		wronganswer = 0;
		gametype = type;
		difficulty = diff;	
		score = 0;
		pressedkeys = 0;
		correctkeys = 0;
		keystrokespersec = 0;
		wordmiss = 0;
		maxnumberofwords = 50; 	
		maxwordmiss = 9;
		accuracy = 100;
		highscore = true;
		clock_gettime(CLOCK_REALTIME, &gamestarttime);

	}
	friend ostream& operator<< (ostream &out, gameinfo &gameref);
	
	struct timespec getstarttime()
	{
		return gamestarttime;
	}	
	
	double getelapsedtime()
	{
		struct timespec curtime; // starttime for the game.
		clock_gettime(CLOCK_REALTIME, &curtime);
		double gametime = (curtime.tv_sec-gamestarttime.tv_sec) + ( curtime.tv_nsec - gamestarttime.tv_nsec ) / 1E9;
		return gametime;
	}
		
	double getaccuracy()
	{
		return accuracy;
	}	
		
	int getmaxnumberofwords()
	{
		return maxnumberofwords;
	}
	int getmaxwordmiss()
	{ 
		return maxwordmiss;
	}
	int getgametype()
	{
		return gametype;
	}
	int getdifficulty()
	{
		return difficulty;
	}
	int getwordmiss()
	{
		return wordmiss;
	}
	void addwordmiss()
	{
		wordmiss++;
	}
	
	int getwronganswers()
	{
		return wronganswer;
	}
	int getcorrectkeys()
	{
		return correctkeys;
	}
	int getkeystrokes()
	{
		return pressedkeys; 
	}
	bool gethighscoreenabled()
	{
		return highscore;
	}
	double getCPSvalue()
	{
		return ( ((double) pressedkeys) / getelapsedtime());
	}
	
	double getEPMvalue()
	{
		return (double) (60*correctkeys) / getelapsedtime();
	}
	double getEaccuracy()
	{
		return wronganswer;//100*(double)correctkeys / (correctkeys + wrongkeys);
	}
	
	int getscore()
	{
		return score;
	}
	void setgametype(int number)
	{
		gametype = number;
	}
	void sethighscore(bool sethighscore)
	{
		highscore = sethighscore;
	}
	
	void setdifficulty(int number)
	{
		difficulty = number;
	}
	
	void addwronganswers()
	{
		wronganswer++;
	}
	void addscore(int number)
	{
		score = score + number;
	}
	
	void addkeystrokes(int number)
	{
		pressedkeys = pressedkeys + number; 
	}
	
	void addkeystroke()
	{
		pressedkeys++;
	}
	void updateaccuracy()
	{
		accuracy = (100*correctkeys) / (double)pressedkeys;
	} 
	
	void addcorrectkeys(int number)
	{
		 correctkeys = correctkeys + number;
		 accuracy = (100*correctkeys) / (double)pressedkeys;		 
	}
	string getrank(string &rank)
	{
		if (score < 500){ rank = "Beginner";}
		else if ((score >= 500) ){rank = "Learner";}
		else if ((score >= 1000)){rank = "Medium";}
		else if ((score >= 1250)){rank = "Good";}
		else if ((score >= 1500)){rank = "Excellent";}
		else if ((score >= 1750)){rank = "Insane";}
		else if ((score >= 2000)){rank = "God Like";}				
		return rank;
	}
};

ostream& operator<< (ostream &out, gameinfo &gameref)
{



    out << "( com = " ;//<< gameref.commandlineword << ", scor = " << endl<<
		//gameref.score << ", presskey = " << gameref.pressedkeys << endl <<
		//"correctkey = " << gameref.correctkeys << " )" << endl;
       // cPoint.m_dZ << ")";
    return out;
}

int removespace(string &str)
{
	str.erase(0, str.find_first_not_of(' '));       //remove spaces
	str.erase(str.find_last_not_of(' ')+1);
	return 1;        
	
}

void setcolor(int color)
{
	attron(COLOR_PAIR(color));
}

int newhighscore(int gametime, gameinfo &gameref)
{
	char tempplayername[24];
	string playername;
	erase();
	timeout(-1); // no we do wait for user input.
	(void)echo(); // we want to show the pressed keys on the screen
	attron(COLOR_PAIR(4));
	attron(A_BOLD);
	mvprintw(3,15,"Enter Your Name ");
	mvprintw(4,19," -> ");
	attroff(A_BOLD);

	attron(COLOR_PAIR(1));
	
	attron(COLOR_PAIR(4));
	attron(A_BOLD);
	mvprintw(7,15,"Game Results");
	attroff(A_BOLD);
	attron(COLOR_PAIR(6));
	mvprintw(9,15,"Score:");
	mvprintw(10,15,"Rank:");
	
	attron(COLOR_PAIR(3));

	if (gameref.getscore() < 750)      { attron(COLOR_PAIR(2)); } 
	else if (gameref.getscore() < 1500) { attron(COLOR_PAIR(5)); } 
	else if (gameref.getscore() < 200000) { attron(COLOR_PAIR(1)); }
	mvprintw(9,30,"%d",gameref.getscore());
	string rankout;
	gameref.getrank(rankout);
	mvprintw(10,30,"%s",rankout.c_str());
	
	attron(COLOR_PAIR(6));

	if (gameref.getgametype() == 1)
	{
		mvprintw(11,15,"CPS:");
		if (gameref.getCPSvalue() < 1.5) { attron(COLOR_PAIR(2)); } 
		else if (gameref.getCPSvalue() < 2.5) { attron(COLOR_PAIR(5)); } 
		else if (gameref.getCPSvalue() < 1000) { attron(COLOR_PAIR(1)); }
	} else if (gameref.getgametype() == 0)
	{	
		mvprintw(11,15,"EPM:");
		if (gameref.getCPSvalue()*60 < 40) { attron(COLOR_PAIR(2)); } 
		else if (gameref.getCPSvalue() < 80) { attron(COLOR_PAIR(5)); } 
		else if (gameref.getCPSvalue() < 10000) { attron(COLOR_PAIR(1)); }
	}							
	if (gameref.getgametype() == 0)	
	{
		if (60*gameref.getCPSvalue()<10)
		{
			mvprintw(11,28,"%3.0f",60*gameref.getCPSvalue());
		} else if (60*gameref.getCPSvalue()<100)
		{
			mvprintw(11,29,"%3.0f",60*gameref.getCPSvalue());
		} else if ((60*gameref.getCPSvalue()>=100))
		{
			mvprintw(11,30,"%3.0f",60*gameref.getCPSvalue());
		}
	}
	if (gameref.getgametype() == 1)	{mvprintw(11,30,"%1.1f",gameref.getCPSvalue());}

	
	attron(COLOR_PAIR(6));
	mvprintw(12,15,"Accuracy:");
	if (gameref.getaccuracy() < 80)       { attron(COLOR_PAIR(2)); } 
	else if (gameref.getaccuracy() < 95)  { attron(COLOR_PAIR(5)); } 
	else if (gameref.getaccuracy() <= 100) { attron(COLOR_PAIR(1)); }							
	mvprintw(12,30,"%3.0f",gameref.getaccuracy());

	attron(COLOR_PAIR(6));
	if (gameref.getgametype() == 0)	{mvprintw(13,15,"Equations:");}
	if (gameref.getgametype() == 1)	{mvprintw(13,15,"Keystrokes:");}
	if (gameref.getgametype() == 0)
	{
		if (gameref.getcorrectkeys() < 2*40)       { attron(COLOR_PAIR(2)); } 
		else if (gameref.getcorrectkeys() < 2*70)  { attron(COLOR_PAIR(5)); } 
		else if (gameref.getcorrectkeys() < 2*100) { attron(COLOR_PAIR(1)); }
	} else if (gameref.getgametype() == 0)
	{
		if (gameref.getcorrectkeys() < 300)       { attron(COLOR_PAIR(2)); } 
		else if (gameref.getcorrectkeys() < 500)  { attron(COLOR_PAIR(5)); } 
		else if (gameref.getcorrectkeys() < 5*1000000) { attron(COLOR_PAIR(1)); }
	}
	mvprintw(13,30,"%d",gameref.getcorrectkeys());					
	mvprintw(4,23,"");
	attron(A_BOLD);
	attron(COLOR_PAIR(1));
	//if (gameref.getscore() < 750)      { attron(COLOR_PAIR(2)); } 
	//else if (gameref.getscore() < 1500) { attron(COLOR_PAIR(5)); } 
	//else if (gameref.getscore() < 200000) { attron(COLOR_PAIR(1)); }
	// print all the info from the game
	
	
	
	refresh();
	while (playername.size()<1)
	{
		mvprintw(4,23,"");
		getstr(tempplayername);
		playername = tempplayername;
		playername.erase(0, playername.find_first_not_of(' '));       //remove spaces
		playername.erase(playername.find_last_not_of(' ')+1);        	
	}
	attron(A_BOLD);
	erase();
	string HighScoreName;
	string TempHighScoreName;
	
	HighScoreName.append("HighScore/HighScore");
	TempHighScoreName.append("HighScore/TempHighScore");
	stringstream Highres;
	Highres << gameref.getgametype(); 
	Highres << gameref.getdifficulty();
	string strhigh = Highres.str();
	HighScoreName.append(strhigh);
	TempHighScoreName.append(strhigh);
	HighScoreName.append(".txt");
	TempHighScoreName.append(".txt");				
					
	string rank;
	// Place the rank
	
	
	ofstream ohighscorefile (TempHighScoreName.c_str(),fstream::out | fstream::app);				
	ifstream ihighscorefile (HighScoreName.c_str());
	
	
	if (!ihighscorefile.is_open())
	{

		if (gameref.getgametype()==1)
		{	
			string rank; 
			gameref.getrank(rank);

			ohighscorefile << setprecision(2) <<   gameref.getscore() << " # " << playername << " # " 
			<<  rank << " # "<< (int)gameref.getaccuracy() << " # " 
			<< gameref.getCPSvalue() << endl;									
		} else 			
		{
			string rank; 
			gameref.getrank(rank);

				ohighscorefile << setprecision(2) << gameref.getscore() << " # " 
						<< playername << " # " <<  rank << " # " << (int)gameref.getaccuracy() << " # " 
						<< 60*gameref.getCPSvalue() <<  endl;
		}			
	}
	
	if (ihighscorefile.is_open() and ohighscorefile.is_open()) // display the new highscore list
	{
		int i=1;
		int value =2;
		//mvprintw(3,15,"HighScores:");
		bool hasbeenwritten = false;
		string line;
		//bool fileEmpty = true; 
		
		
		while (getline (ihighscorefile,line) )//getline (ihighscorefile,line) )
		{
			//fileEmpty = false;
			// compare the scores
			string templine;

			size_t pos = line.find("#");

			templine = line.substr(0,pos-1);
			
			templine.erase(0, templine.find_first_not_of(' '));       //remove spaces
			templine.erase(templine.find_last_not_of(' ')+1);        

			
			int value = atoi(templine.c_str());
												
			if (value > gameref.getscore() or hasbeenwritten)
			{
				ohighscorefile << line << endl;

			} else 
			{
				hasbeenwritten = true;
				if (gameref.getgametype() == 1) // (word mode)
				{													
					string rank; 
					gameref.getrank(rank);

					ohighscorefile << setprecision(2) <<   gameref.getscore() << " # " << playername << " # " 
					<<  rank << " # "<< (int)gameref.getaccuracy() << " # " 
					<< gameref.getCPSvalue() << endl;									
				} else if (gameref.getgametype() == 0) // (math mode) 
				{
					
					string rank; 
					gameref.getrank(rank);
					ohighscorefile << setprecision(2) << gameref.getscore() << " # " 
							<< playername << " # " <<  rank << " # " << (int)gameref.getaccuracy() << " # " 
							<< 60*gameref.getCPSvalue() <<  endl;
				}
				ohighscorefile << line << endl;
				
			}						
		}
		if (!hasbeenwritten)
		{
			hasbeenwritten = true;
			if (gameref.getgametype() == 1) // (word mode)
			{													
				string rank; 
				gameref.getrank(rank);

				ohighscorefile << setprecision(2) <<   gameref.getscore() << " # " << playername << " # " 
				<<  rank << " # "<< (int)gameref.getaccuracy() << " # " 
				<< gameref.getCPSvalue() << endl;									
			} else if (gameref.getgametype() == 0) // (math mode) 
			{
				
				string rank; 
				gameref.getrank(rank);
				ohighscorefile << setprecision(2) << gameref.getscore() << " # " 
						<< playername << " # " <<  rank << " # " << (int)gameref.getaccuracy() << " # " 
						<< 60*gameref.getCPSvalue() <<  endl;
			}
			ohighscorefile << line << endl;
			
		}
		

		ihighscorefile.close();
		ohighscorefile.close();
	} else 
	{
			mvprintw(LINES-1,COLS/3,"Could not open High Score file.");
	}
	
	rename( TempHighScoreName.c_str() , HighScoreName.c_str() );
					
}


int showhighscore(gameinfo &gameref);



void Initialise()
{
	// initialises the settings for the console and ncurses functions.
    initscr(); // initialise the screen
    (void)noecho();// dont echo the input to the screen
	timeout(0); // dont wait for user input
	//start_color();
	//init_pair(1, COLOR_CYAN, COLOR_BLACK);

}

void checkword(string iword, vector<Word*> iowordlist, int ioscore)
{
	int a;
	// check if the word is in the wordlist shown on screen
	for (a=0;a<iowordlist.size();a++)
	{
		if (iowordlist[a]->getword()==iword)
		{
			ioscore=ioscore + iowordlist[a]->getvalue();
			iowordlist.erase(iowordlist.begin()+a);
		}
	}
}

int findcolumn(vector<Word*> &wordlist)
{
	bool newlinenotfound = true;
	int linenumber = rand() % (LINES-3) + 1;
	if (wordlist.size() >= (LINES-3)) // If there are more words then lines, we just pick a random line.
	{
		return linenumber;
	}
	
	int breakout;
	while (true)
	{
		breakout = 0;
		for (int i=0;i<wordlist.size();i++)
		{
			if (linenumber == wordlist[i]->getrow())
			{
				linenumber = rand() % (LINES-3) + 1;
				breakout = 1;
				break;
			}			
		}
		if (breakout == 0){break;}
	}
	
	return linenumber;
	
}

int checkforword(vector<Word*> &wordlist, gameinfo &gameref, commandline &cmdline)
{
	for (int a=0;a<wordlist.size();a++)
	{
		if (gameref.getgametype() == 1) // Word mode
		{
			if (wordlist[a] -> getword() == cmdline.getcommandline())
			{
				gameref.addcorrectkeys(cmdline.getcommandline().size());  // presses = correctkeypresses + ch.size();							 
				gameref.addscore(wordlist[a]->getvalue());  // = score + wordlist[a]->getvalue();
				wordlist.erase(wordlist.begin()+a);
				break;
			} else 
			{ 
				gameref.updateaccuracy();
			}
		} else if (gameref.getgametype() == 0)  // Math mode
		{
			//string tempa = wordlist[a] -> getresult();
			if (wordlist[a] -> getresult()==cmdline.getcommandline())
			{
				gameref.addscore(wordlist[a]->getvalue());
				gameref.addcorrectkeys(cmdline.getcommandline().size());
				wordlist.erase(wordlist.begin()+a);
				break;
			} else 
			{
				gameref.updateaccuracy();
			} 										
		}
	}
}

int inputactions(vector<Word*> &wordlist, gameinfo &gameref, commandline &cmdline)
{
	struct timespec curTime;
	struct timespec keyStart;
	bool noupdate = true;
	double timeinms = 0;
	clock_gettime(CLOCK_REALTIME, &keyStart);
	
	while (noupdate && (timeinms < 50))  // waiting for user input for 50 ms then continuing to update the program. 
	{
		int chr = getch();
		clock_gettime(CLOCK_REALTIME, &curTime);
		
		timeinms = (curTime.tv_sec - keyStart.tv_sec)*1000 + ( curTime.tv_nsec - keyStart.tv_nsec  ) / 1E6; //elaspedtime - starttime;

		switch(chr) 
		{   // OBS when the player presses space or enter the game can run through this part multiple times.
			case 27:
				erase();
				attron(COLOR_PAIR(5));
				attron(A_BOLD);
				mvprintw(LINES/2-3,COLS/2-8,"GAME PAUSED");
				attroff(A_BOLD);
				attron(COLOR_PAIR(6));
				mvprintw(LINES/2,COLS/2-14,"press a key to continue.");
				refresh();
				timeout(-1); // set the waiting time to infinite
				getch();     // wait for the user to press a key
				timeout(0);	 // set the waiting time back to zero
				break;
				
			case 3: // ctrl c exits the program
				endwin();
				exit(0);
			case 10: //enter_key evaluates the word 
				checkforword(wordlist,gameref,cmdline);
				cmdline.clearcommandline();
				noupdate = false;
				break;
			
			case 32:  // spacekey evaluates the word
				checkforword(wordlist,gameref,cmdline);
				cmdline.clearcommandline();
				noupdate = false;
				break;
			case 127: // backspace_key skips the last element in the comandlinestring.
				cmdline.removelastincommandline();
				break;
			case 410:
				break; // I dont know where if gets this input from (~Z), but im not using it.
			default:
				if (chr != -1) // any other character are added to the ch character string
				{
					//ch += chr;
					cmdline.addtocommandlineword(chr);
					gameref.addkeystroke();//totalkeypresses++;
					noupdate = false;										
				}
		}
	}
	return 1;
}

int loadwords(vector<string> &allwords, gameinfo &gameref)
{
	string filename;
	try
	{
		ifstream infile;
		string input;
		
		switch (gameref.getdifficulty())
		{
			case 0:
				filename ="Wordlists/EasyWordlist.txt";				
				break;
			case 1:
				filename ="Wordlists/MediumWordlist.txt";
				break;
			case 2:
				filename ="Wordlists/HardWordlist.txt";
				break;
		}
		infile.open(filename.c_str());
		while (infile >> input)
		{
			allwords.push_back(input); // read all the input text into a vector
		}
		if (allwords.size() < 1){throw filename;}
		return 0;
	}
	catch (string filename)
	{
		erase();
		mvprintw(LINES-(LINES/2+2),COLS/6,"No word where read from the textfile: %s ",filename.c_str());
		mvprintw(LINES-(LINES/2+1),COLS/6,"Maybe the textfile has been deleted or renamed.");
		refresh();
		napms(8000); // Wait 8 seconds so the player can read the message
		return 1;	
	}
}

void loadquestions(vector<string> &allwords, gameinfo &gameref)
{
	vector<string> allquestions;
	vector<string> allanswers;
	try
	{
		ifstream infile("Questions.txt");
		string question,answer,input;
		while (infile >> input)
		{
			size_t pos = input.find("#");
			question = input.substr(0,pos-1);
			question.erase(0, question.find_first_not_of(' '));       //remove spaces
			question.erase(question.find_last_not_of(' ')+1);        
					
			answer = input.substr(pos+1);
			answer.erase(0, question.find_first_not_of(' '));       
			answer.erase(question.find_last_not_of(' ')+1);         			
		
			allquestions.push_back(question);
			allanswers.push_back(answer); 
		}
		if ((question.size()==0) or (answer.size()==0))
		{
			throw 1;
		}
		if (!(question.size()==answer.size()))
		{
			throw 2;
		}
	}
	catch (int x)
	{
		if (x==1)
		{
			erase();
			mvprintw(LINES-LINES/2,COLS/3,"No word where read from the textfile.");
			refresh();
			napms(2000);	
			//return 1;				
		}
		if (x==2)
		{
			erase();
			mvprintw(LINES-LINES/2,COLS/3,"There where not the same number of answers and questions.");	
			refresh();
			napms(2000);	
			//return 1;				
		}
	}			
}

int findnewcolumn(vector<Word*> &wordlist)
{
	int maxindent = 0;
	int wordlistrow = 0;
	int k = LINES-3;
	int columnnumber;
	vector<int> indentlist(LINES-3);
	for (int a = 0; a < (LINES-3); a++)
	{
		indentlist[a] = COLS + 10;
		
	}

	for (int a = 0 ; a<wordlist.size(); a++)
	{
		if (indentlist[wordlist[a]->getrow()-1] > wordlist[a]->getindent())
			{
				indentlist[wordlist[a]->getrow()-1] = wordlist[a]->getindent();
			}
	}

	for (int a = 0; a < (LINES-3); a++)
	{
		if (indentlist[a] >  maxindent)
		{
			maxindent = indentlist[a];
			columnnumber = a+1;
		}
	}
	return columnnumber;
}

void insertword(vector<Word*> &wordlist,vector<string> allwords, gameinfo &gameref)
{
	if ( wordlist.size() < gameref.getmaxnumberofwords() )
	{

		if (gameref.getgametype()==1) // Wordmode
		{
			int tempp = rand() % allwords.size();
			
			// Find a line thats not already used on the display
			int columnnumber;
			if (wordlist.size() < (LINES-3)) 
			{
				columnnumber = findcolumn(wordlist);
			} else 
			{
				columnnumber = findnewcolumn(wordlist);
			}
			
			
			Word* word = new Word (allwords[tempp],columnnumber); // create a new word to place in the wordlist

			allwords.erase(allwords.begin()+tempp); // remove the word from the all words list so it's always different words.
			wordlist.push_back(word);
		
		} else if (gameref.getgametype()==0) // Mathmode
		{
			// need to create the mathematical expression
			// pick operator
			// 0 -> +
			// 1 -> -
			// 2 -> *
			// 3 -> /
			// 4 -> % 
			int oper;
			int numberrange;
			
			switch (gameref.getdifficulty())
			{
			case 0:
				oper = rand() % 2;
				numberrange = 3 *pow((2-oper)+1,2);
				break;
			case 1:					
				oper = rand() % 3;
				numberrange = 6 *pow((3-oper)+1,2);
				break;
			case 2:					
				oper = rand() % 3;	
				numberrange = 10 *pow((4-oper)+1,2);
				break;
			case 3:
				oper = rand() % 3;	
				numberrange = 20 *pow((5-oper)+1,2);
				break;
			}
			
			bool bigoper = false;
			int result;
			
			if (oper > 2)
			{
				bigoper = true;
			}
			
			
			int ra = rand() % numberrange;
			int rb = rand() % numberrange;
			
			if (oper == 3)
			{
				ra = ra*rb;
			}
		
			while ( oper == 4 )
			{
				if (rb !=0)
				{
					if ((ra > rb) and (ra % rb) == 0)
					{
						break;
					}
				}
				ra = rand() % numberrange;
				rb = rand() % numberrange;
			}
			
			stringstream ssa;
			ssa << ra;
			string stra = ssa.str();

			stringstream ssb;
			ssb << rb;
			string strb = ssb.str();

			string str;
			str.append(stra);
			
			switch (oper)
			{
				case 0:
					str.append(" + ");
					result = ra + rb;
					break;
				case 1:
					str.append(" - ");
					result = ra - rb;
					break;
				case 2:
					str.append(" * ");
					result = ra * rb;
					break;
				case 3:
					str.append(" / ");
					result = ra / rb;
					break;
				case 4:
					str.append(" % ");
					result = ra % rb;
					break;

			}

			stringstream ssres;
			ssres << result;
			string strres = ssres.str();
			
			str.append(strb);
			str.append(" =");			 
						
			int columnnumber;
			if (wordlist.size() < (LINES-3)) 
			{
				columnnumber = findcolumn(wordlist);
			} else 
			{
				columnnumber = findnewcolumn(wordlist);
				
				
			}
			
			Word* word = new Word (str, columnnumber ,2,strres); // create a new word to place in the wordlist
			wordlist.push_back(word);
		} else if (false) // TODO, are implemented later
		{
			int tempp = rand() % allwords.size();
			string foo;			
			//Word* word = new Word (allquestions[tempp],rand() % (LINES-3) + 1,1,allanswers[tempp]); // create a new word to place in the wordlist

			//allquestions.erase(allwords.begin()+tempp); // remove the word from the all words list so it's always different words.
			//allanswers.erase(allwords.begin()+tempp);
			//wordlist.push_back(word);
			
		}
	} else if (wordlist.size() < gameref.getmaxnumberofwords())
	{
		
		}
}

void printtoscreen(vector<Word*> &wordlist,vector<string> &allwords, gameinfo &gameref, commandline &cmdline)
{
	for (int a=0;a<wordlist.size();a++)
	{
		if ((wordlist[a]->getindent()+wordlist[a]->getword().size()) > ((4*COLS)/5))
		{
			attron(COLOR_PAIR(2));
			attron(A_BOLD);
			mvprintw(wordlist[a]->getrow(),wordlist[a]->getindent(),"%s",wordlist[a]->getword().c_str());
			attroff(A_BOLD);
			attron(COLOR_PAIR(1));
		} else 
		{
			attron(A_BOLD);
			attron(COLOR_PAIR(1));
			mvprintw(wordlist[a]->getrow(),wordlist[a]->getindent(),"%s",wordlist[a]->getword().c_str());
			attroff(A_BOLD);
		}
	}		
	attron(COLOR_PAIR(6));
	mvprintw(LINES-1,2,"> ");
	attron(COLOR_PAIR(1));
	attron(A_BOLD);
	mvprintw(LINES-1,4,"%s",cmdline.getcommandline().c_str()); // print the string we are currently typing on
	attroff(A_BOLD);
	
	if ((COLS/4 > (cmdline.getcommandline().size()+3)) && (COLS/3 > 8)) // 'Miss x' are at assume to fill atmost 8 chracters.
	{ 
		attron(COLOR_PAIR(6));
		mvprintw(LINES-1,(2*COLS)/5+5,"Miss: ");
					
		if (gameref.getwordmiss() < 4)  {attron(COLOR_PAIR(1));} else 
		if (gameref.getwordmiss() <  7) {attron(COLOR_PAIR(5));} else 
		if (gameref.getwordmiss() >= 7) {attron(COLOR_PAIR(2));}
		
		printw("%d",gameref.getwordmiss());
		
		attron(COLOR_PAIR(1));
		
		if (COLS/3 > (15)) // the 'Score xx' is assumed to fill less than 10 charcters
		{			
			
			attron(COLOR_PAIR(6));
			mvprintw(LINES-1,COLS/5+6,"Score: ");
		
			if (gameref.getscore() < 750)  {attron(COLOR_PAIR(2));} else 
			if (gameref.getscore() < 1500 ) {attron(COLOR_PAIR(5));} else 
			if (gameref.getscore() >= 1500) {attron(COLOR_PAIR(1));}
			printw("%d",gameref.getscore());
			attron(COLOR_PAIR(6));
		
			mvprintw(LINES-1,(3*COLS)/5+2,"Acc: ");
			
			if (gameref.getaccuracy() < 80)  {attron(COLOR_PAIR(2));} else 
			if (gameref.getaccuracy() < 95 ) {attron(COLOR_PAIR(5));} else 
			if (gameref.getaccuracy() >= 95) {attron(COLOR_PAIR(1));}

			printw("%3.0f", gameref.getaccuracy()) ;
			
			attron(COLOR_PAIR(6));
			if (gameref.getgametype() == 0)
			{mvprintw(LINES-1,(4*COLS)/5+1,"EPM: ");}
			else if (gameref.getgametype() == 1)
			{mvprintw(LINES-1,(4*COLS)/5+1,"CPS: ");}
			if (gameref.getCPSvalue() < 1.5)  {attron(COLOR_PAIR(2));} else 
			if (gameref.getCPSvalue() < 2.7 ) {attron(COLOR_PAIR(5));} else 
			if (gameref.getCPSvalue() >= 2.7) {attron(COLOR_PAIR(1));}
			//printw("%f", gameref.getelapsedtime());
			
			if (gameref.getgametype() == 0)
			{
				printw("%3.0f", 60*gameref.getCPSvalue());
			} else if (gameref.getgametype() == 1)
			{
				printw("%1.1f", gameref.getCPSvalue());
			}
			//printw("%d", gameref.getkeystrokes());
			attron(COLOR_PAIR(1));
		}
	}
	
		
	else 
	{
		if ((COLS/2 > (cmdline.getcommandline().size()+3)) && (COLS/3 > 8))
		{
			mvprintw(LINES-1,COLS/2,"Miss %d",gameref.getwordmiss());			
		}
	}
	
	
	string displayline = string(COLS,'_');
	attron(COLOR_PAIR(3));
	mvprintw(LINES-2,0,displayline.c_str()); // draw line over the buttom info line
	attron(COLOR_PAIR(1));
	refresh();

}

int startgame(int gametype, int difficulty)
{
	//gameinfo *gameref;
	
	gameinfo *gameref = new gameinfo(gametype,difficulty); // object holding the game informations, the score, key pressed, word missed and so on..
	commandline *cmdline = new commandline(); // object holding the commandline 
	vector<string> allwords; // string where all the words are stored (from a text file). 
	vector<string> allquestions; // to be made for questions/answers.
	vector<string> allanswers;  // to be made for questions/answers.
	vector<Word*> wordlist;

	// the settings
	//-------------------------
	
	int indenttime = 375;  		// timeinterval between the indent of the words
	int newwordtime = 4000;		// timeinterval between new words are added
	const int keytime = 20;		// timeinterval maximum waiting for user input
	
	//------------------------
	
	cout << "\e[8;20;80;t"; //resize the terminal window

	//Initialise();
	srand (time(NULL));
	// initialises the settings for the console and ncurses functions.
    //initscr(); // initialise the screen
    (void)noecho();// dont echo the input to the screen
	timeout(0); // dont wait for user input
	
	curs_set(0);
	refresh();

	if (gameref->getgametype()==1) // word mode
	{
		if (loadwords(allwords,*gameref) == 1) 
		{
			return 0;
		}
	}
	
	
	if (false) // not implemented yet	
	{
		loadquestions(allwords,*gameref);
	}	
	//showhighscore(gametype, difficulty);

	// Timers to keep track of the time spend...
	
	// structures to hold the time that have two entrainces
	// one holding the number of seconds and one holding a number of nanoseconds
	// the time is numberseconds + numbernanoseonds
	// the seconds are hold in .tv_sec e.g. keyStart.tv_sec
	// the nanoseconds are hold in .tv_nsec e.g. keyStart.tv_nsec
	struct timespec keyStart;
	struct timespec indentStart;
	struct timespec nextwordStart;
	struct timespec curTime;
	struct timespec gameStart;
	struct timespec gameStop;


	// here the current time is read into the structure.
	clock_gettime(CLOCK_REALTIME, &indentStart);
	clock_gettime(CLOCK_REALTIME, &nextwordStart);
	clock_gettime(CLOCK_REALTIME, &gameStart);
				
	// The while loop that runs the game.
	while (true)
	{		
		clock_gettime(CLOCK_REALTIME, &keyStart);

		double timeinms = 0;
		bool noupdate = true;
		
		inputactions(wordlist,*gameref, *cmdline); // 
				
		noupdate = true;
		timeinms = 0;

		refresh();
		erase();
	
		clock_gettime(CLOCK_REALTIME, &curTime);
		
		double dtimems = (curTime.tv_sec-indentStart.tv_sec)*1000 + ( curTime.tv_nsec - indentStart.tv_nsec ) / 1E6;

		if( dtimems > indenttime ) // Iindent the words every indenttime interval
		{
			clock_gettime(CLOCK_REALTIME, &indentStart);
			for (int a=0;a<wordlist.size();a++)
			{
				wordlist[a]->increaseIndent();
				if (wordlist[a]->getindent() > COLS - wordlist[a]->getlength()) // if the words gets out of the screen it is deleted.
				{							
					beep(); // <- make a beep sound so the player know that the word is missed
					wordlist.erase(wordlist.begin()+a);
					gameref->addwordmiss();
					a--;															
				}
			}
		}
		
		if (gameref->getwordmiss()  > gameref->getmaxwordmiss()) // the number of misses berfore the game ends
		{
				// go to enter name screen				
				clock_gettime(CLOCK_REALTIME, &gameStop);
				double gametime = (gameStop.tv_sec-gameStart.tv_sec) + ( gameStop.tv_nsec - gameStart.tv_nsec ) / 1E9;
				if (gameref->gethighscoreenabled())
				{
					newhighscore(gametime,*gameref);
				}
				showhighscore(*gameref);
				break;				
		}
		
				
		printtoscreen(wordlist,allwords, *gameref, *cmdline); // now the words are printed to the screen.
		clock_gettime(CLOCK_REALTIME, &curTime);
		double timems = (curTime.tv_sec-nextwordStart.tv_sec)*1000 + ( curTime.tv_nsec  - nextwordStart.tv_nsec ) / 1E6;
		if ((timems > newwordtime) || (wordlist.size()==0) ) // If we went through enough cycles then place a new word
		{   
			clock_gettime(CLOCK_REALTIME, &nextwordStart);
			insertword(wordlist,allwords, *gameref);
			newwordtime = (0.96 * newwordtime + 50 );
			indenttime = indenttime - 1;
		}
	}
	erase();
	return 0;
}


int showhighscore(gameinfo &gameref)
{
	(void)noecho();
	erase();
	int linenr=0;
	string HighScoreName;
	
	HighScoreName.append("HighScore/HighScore");
	stringstream Highres;
	Highres << gameref.getgametype(); 
	Highres << gameref.getdifficulty();
	string strhigh = Highres.str();
	HighScoreName.append(strhigh);
	HighScoreName.append(".txt");
			
	int i=0;
	string line;
	int k;
	int key;
	int maxlinenum = 0;
	bool NoKeyPressed;
	bool ShowHighSc = true;

	while (ShowHighSc)
	{
		erase();
		refresh();

		attron(A_BOLD);
		attron(COLOR_PAIR(4));
		mvprintw(2-linenr,COLS/2-5,"High Score");
		attroff(A_BOLD);
		switch(gameref.getgametype())
		{
			case 0:
				mvprintw(3-linenr,COLS/2-10,"Math game");
				break;
			case 1:
				mvprintw(3-linenr,COLS/2-10,"Word game");
				break;
			
		}
		
		switch(gameref.getdifficulty())
		{
			case 0:
				mvprintw(3-linenr,COLS/2,"easy mode");
				break;	
			case 1:
				mvprintw(3-linenr,COLS/2,"normal mode ");
				break;	
			case 2:
				mvprintw(3-linenr,COLS/2,"hard mode");
				break;	
			case 3:
				mvprintw(3-linenr,COLS/2,"expert mode");
				break;	
		}
		


		attron(A_BOLD);
		mvprintw(5-linenr,7,"Name");		
		attron(COLOR_PAIR(1));
		mvprintw(5-linenr,30,"Score");
		attron(COLOR_PAIR(3));
		mvprintw(5-linenr,3,"#");
		attron(COLOR_PAIR(1));
		mvprintw(5-linenr,7,"Name");
		mvprintw(5-linenr,40,"Rank");
		mvprintw(5-linenr,55,"Acc[%]");
		
		if (gameref.getgametype() == 1) {mvprintw(5-linenr,66,"CPS");}
		if (gameref.getgametype() == 0) {mvprintw(5-linenr,66,"EPM");}
		attroff(A_BOLD);
		ifstream inewhighscorefile (HighScoreName.c_str());
		if (inewhighscorefile.is_open()) // display the new highscore list
		{
			
			
			int i = 0;
			while ( getline (inewhighscorefile,line) )
			{			
				if (line.size() < 2) {continue;}
				i++;
				if (i > maxlinenum)
				{
						maxlinenum = i;
				}
				
				string score;
				string name;
				string rank;
				string accuracy;
				string CPS;
				
				string value;
				string rest;				
			
				size_t pos = line.find("#");
				int counter = 0;
				while (pos != -1) // while there still is a # in the string
				{
					if (counter == 0){
						score = line.substr(0,pos-1);
						removespace(score);
						line = line.substr(pos+1);
						pos = line.find("#");} 
					else if (counter == 1){
						name = line.substr(0,pos-1);
						removespace(name);
						line = line.substr(pos+1);
						pos = line.find("#");}
				
					else if (counter == 2){
						rank = line.substr(0,pos-1);
						removespace(rank);
						line = line.substr(pos+1);
						pos = line.find("#");}

					else if (counter == 3){
						accuracy = line.substr(0,pos-1);
						removespace(accuracy);
						line = line.substr(pos+1);
						pos = line.find("#");}

					counter++;						
				}
				CPS = line;
				removespace(CPS);
				
				attron(COLOR_PAIR(1));
				mvprintw(i+6-linenr,7,"%s",name.c_str());
				attron(COLOR_PAIR(6));
				mvprintw(i+6-linenr,3,"%d",i);
				int sivalue = atoi(score.c_str());
				if (sivalue < 1000)      { attron(COLOR_PAIR(2)); } 
				else if (sivalue < 1500) { attron(COLOR_PAIR(5)); } 
				else if (sivalue < 200000) { attron(COLOR_PAIR(1)); }
								
				mvprintw(i+6-linenr,30,"%s",score.c_str());
				mvprintw(i+6-linenr,40,"%s",rank.c_str());
				
				
					
				float sfvalue = atof(accuracy.c_str());
				
				if (sfvalue < 80)       { attron(COLOR_PAIR(2)); } 
				else if (sfvalue < 95)  { attron(COLOR_PAIR(5)); } 
				else if (sfvalue < 101) { attron(COLOR_PAIR(1)); }							
				mvprintw(i+6-linenr,55,"%s",accuracy.c_str());
				
				
				sfvalue = atof(CPS.c_str());
				if (gameref.getgametype() == 1)
				{
					if (sfvalue < 1.5) { attron(COLOR_PAIR(2)); } 
					else if (sfvalue < 2.5) { attron(COLOR_PAIR(5)); } 
					else if (sfvalue < 101) { attron(COLOR_PAIR(1)); }
				} else if (gameref.getgametype() == 0)
				{	
					if (sfvalue < 1.5*20) { attron(COLOR_PAIR(2)); } 
					else if (sfvalue < 2.5*20) { attron(COLOR_PAIR(5)); } 
					else if (sfvalue < 1000*20) { attron(COLOR_PAIR(1)); }
				}							
				mvprintw(i+6-linenr,66,"%s",CPS.c_str());											
			}
		}
		inewhighscorefile.close();
		
		int g;
		NoKeyPressed = true;
		while (NoKeyPressed)
		{
			g++;
			key = getch();
			if (key == 10) 
			{
				ShowHighSc = false;
				break;
			}
			if (key == 3)  // ctrl c exits the program
			{
				endwin();
				exit(0);
			}

			if (key == 66) //258
			{
				NoKeyPressed = false;
				if (linenr < maxlinenum)
				{
					linenr++;
				}
				
			}
			
			if (key == 65) //259
			{
				NoKeyPressed = false;
				if (linenr > 0)
				{
					linenr--;
				}
			}
		}
	}
}



/*
Create the menus for the startup of the game.
The structure is

				easy
*	math  ->	medium	-> start game
				hard

				English
*	words  ->	German	(step with difficulties) -> start game
				Spanish


*	geography 	-> capitals
 

*	history		-> historical events (years)

*/

int printmenu(string menu[], int entries)
{
	bool noselected;
	bool doerase;
	int selecteditem = 0 ;

	string temp;
	erase();
	while (true)
	{

		attron(COLOR_PAIR(3));
		attron(A_BOLD);
		temp = menu[0];
		mvprintw (3,COLS/3, temp.c_str()); // print the menu items
		attroff(A_BOLD);
		int i,j,key;
		for (i=1; i < entries+1 ;i++)
		{		
			temp = menu[i];
			attron(COLOR_PAIR(3));
			mvprintw ((6+2*(i-1)),COLS/3, temp.c_str()); // print the menu items
		}
		
		attron(COLOR_PAIR(5));
		mvprintw ((6+2*selecteditem),COLS/3-3, (char*)"->" );
		
		refresh();
		
		key = getch();


		
		if (key == 3)// ctrl c exits the program
		{
			endwin();
			exit(0);
		}

		if (key == 66)  //
		{

			doerase = true;
			if (selecteditem < (entries - 1))
			{
				selecteditem++;
			}			
		}
		
		if (key == 65)  //
		{

			doerase = true;
			if (selecteditem > 0)
			{
				selecteditem--;
			}
		}

		if (key == 10) // enter key 
		{
			refresh();
			return selecteditem;
			break;		
		}		
		if (key == 113)//q
		{
			return -1;
		}
		if (key == 127)//q
		{
			return -1;
		}
		
			
			
		
		
		if (doerase)
		{
				erase();
				doerase = false;
		}
		
	}
}

int printabout()
{
	erase();
	attron(COLOR_PAIR(5));

	attron(A_BOLD);

	mvprintw(LINES/5-1,COLS/8,"About LearningFun");
	attroff(A_BOLD);
	attron(COLOR_PAIR(5));
	
	mvprintw(LINES/5+2,COLS/8,"This is a game where you can improve"); 
	mvprintw(LINES/5+3,COLS/8,"you your math and typing skills.");
	mvprintw(LINES/5+4,COLS/8,"The game is simple, write the answer before"); 
	mvprintw(LINES/5+5,COLS/8,"the equation/word has propagated"); 
	mvprintw(LINES/5+6,COLS/8,"to the other side of the screen.");

	mvprintw(LINES/5+8,COLS/8,"This game was created by Ole L. Frederiksen"); 
	mvprintw(LINES/5+9,COLS/8,"<ole.lu.frederiksen@gmail.com> in 2013/2014");

    mvprintw(LINES/5+13,COLS/8,"press q or esc to return");
	
	refresh();
	timeout(0);	
	//timeout(-1); 
	while (true)
	{
		int a = getch();
		if (a == 27)// ecs
		{
			return 1;
		}
		if (a == 113)//q
		{
			return 1;
		}

	}
	     
	 

	
}

main(int argc, char *argv[])
{
	//setup the menues.
	int menuselect1 = -1;
	int menuselect2;
	int menuselect3; 
	
	string menustart[] = {
						"Main Menu",
						"Math Game", 
						"Word Game", 
						"About",
						"Quit",                         
						};

    // = 
	// where were I, trying to make something with try except...
	// Its the good old game with not s
    string menuwords[]  = {
					   "Word Menu",
					   "Easy ", 
					   "Medium", 
					   "Hard",
					   "Expert",
					   //"User Defined (No High Score)",
					   "View High Score",
							};
    string menumath[] 	= {
							"Math Menu",
							"Easy ", 
							"Medium", 
							"Hard",
							"Expert",
							//"User Defined (No High Score)",
							"View High Score",
							};
    string menuquestion[] 	= {
							"Hisotry ", 
							"Geography", 
							"User Defined (No High Score)",
							"View High Score",
							};

    string userdefinedmath[] 	= {
							"Number ranges",
							"Start Time Interval",
							"Decrease Rate For Time Interval",
							"Max Number Of Concurrent Equations", 
							"Addition", 
							"Subtraction",
							"Multiplication",
							"Division",
							"Modulus",
							"Play Now",
							};
    string userdefinedwords[] 	= {
							"Start Time Interval",
							"Decrease Rate For Time Interval", 
							"Words with 1-15", 
							"Max Number Of Concurrent Words",
							"Play Now",
							};
	string highscoremenu[]  = {
							"HighScore Menu",
							"Easy ", 
							"Medium", 
							"Hard",
							"Expert",
							};


    initscr(); // initialise the screen
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);   // if colors are to be used, need to be implemented		
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_BLUE, COLOR_BLACK);    //
	init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(5, COLOR_YELLOW, COLOR_BLACK);
	init_pair(6, COLOR_CYAN, COLOR_BLACK);
                          	
    initscr(); 
    noecho();     
    raw();
    curs_set (0);	
    // change the size of the terminal
    cout << "\e[8;20;80;t";

    do 
    { 

		attron(COLOR_PAIR(4));
        menuselect1 = printmenu(menustart,4);
        erase();
        //cout << "menuselect" << menuselect1;
        //napms(1000);		
		if (menuselect1 == -1){break;}
		refresh();
		erase();
		switch(menuselect1)
		{
		case 0:
			menuselect2 = printmenu(menumath,5);
			if (menuselect2 == -1){break;}
		
			if (menuselect2 == 4)
			{
				int selecteddiff = printmenu(highscoremenu,4);
				if (selecteddiff == -1){break;}
				gameinfo *gameref = new gameinfo(menuselect1,selecteddiff);
				showhighscore(*gameref);
				// show highscore
			} else // start game 
			{ 
				startgame(menuselect1,menuselect2);
			}
			break;
		case 1:
			menuselect2 = printmenu(menuwords,5);
			if (menuselect2 == 4)
			{
				int selecteddiff = printmenu(highscoremenu,4);
				gameinfo *gameref = new gameinfo(menuselect1,selecteddiff);
				showhighscore(*gameref);
				// show highscore
			} else // start game 
			{ 
				startgame(menuselect1,menuselect2);
			}
			break;
		case 2:
			printabout();
			erase();
			break;
		case 3:
			endwin();
			exit(0);

		default:
			mvprintw (LINES/3,COLS/3, "not implemented yet"); 						
		} 		
        erase(); 
    } 
    while (menuselect1 != 8);

    endwin();
    return 0;

}
