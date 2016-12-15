#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>

#define NAME_LENGTH 8
#define DEFAULT_NAME "________"
#define RANK_RANGE 10

typedef struct {
  char names[RANK_RANGE][NAME_LENGTH+1];
  int score[RANK_RANGE];
  int player_index;
  int mod_name_index;
  int length;
  int redraw; //1 need, 0 ignore
  int save; //1 save, 0 no
} Rank;

Rank* createRank(int player_score) {
  Rank* rank = (Rank*)malloc(sizeof(Rank));
  rank->length = 0;
  rank->redraw = 1;
  rank->player_index = -1;
  rank->mod_name_index = 0;
  rank->save = 0;

  int i = 0;
  FILE* fp = fopen("snake.data", "r");
  if (fp != NULL) {
    while (0 < fscanf(fp, "%s %d\n", rank->names[i], &(rank->score[i]))) {
      rank->names[i][NAME_LENGTH] = 0;
      i++;
    }
    fclose(fp);
  }
  rank->length = i;

  static char preName[NAME_LENGTH+1];
  static int preScore;
  for (i=0;i<rank->length+1;i++) {
    if (rank->score[i] <= player_score) {
      if (rank->player_index < 0) {
        rank->player_index = i;
        strcpy(preName, rank->names[i]);
        preScore = rank->score[i];
      }
      else {
        char tempName[NAME_LENGTH+1];
        int tempScore = rank->score[i];
        strcpy(tempName, rank->names[i]);

        strcpy(rank->names[i], preName);
        rank->score[i] = preScore;

        strcpy(preName, tempName);
        preScore = tempScore;
      }
    }
  }
  if (rank->length == 0)
    rank->player_index = 0;

  if (rank->player_index >= 0) {
    rank->length++;
    strcpy(rank->names[rank->player_index], DEFAULT_NAME);
    rank->names[rank->player_index][NAME_LENGTH] = 0;
    rank->score[rank->player_index] = player_score;
  }
  if (rank->length > RANK_RANGE)
    rank->length = RANK_RANGE;

  setModNameIndex(rank, 0);
  return rank;
}
void rankSave(Rank* rank) {
  if (rank->mod_name_index >= 0 && rank->mod_name_index < NAME_LENGTH)
    if (rank->names[rank->player_index][rank->mod_name_index] == ' ')
      rank->names[rank->player_index][rank->mod_name_index] = '_';

  FILE* fp = fopen("snake.data", "w");
  for (int i=0;i<rank->length;i++) {
    fprintf(fp, "%s %d\n", rank->names[i], rank->score[i]);
  }
  fclose(fp);

  rank->save = 1;
  rank->player_index = -1;
}
void setModNameIndex(Rank* rank, int index) {
  if (rank->player_index >= 0 && index >= 0 && index < NAME_LENGTH) {
    if (rank->names[rank->player_index][rank->mod_name_index] == ' ')
      rank->names[rank->player_index][rank->mod_name_index] = '_';
    rank->names[rank->player_index][index] = ' ';

    rank->mod_name_index = index;
  }
}
int rankKeyInput(Rank* rank, char c) {
  if (rank->player_index >= 0) {
    if (c == '\n')    //enter
      rankSave(rank);
    else if (c == 67 || c == ' ')   //space or right arrow
      setModNameIndex(rank, rank->mod_name_index + 1);
    else if (c == 68)             //left arrow
      setModNameIndex(rank, rank->mod_name_index - 1);
    else if((c>='A'&&c<='Z')||(c>='a'&&c<='z')) {
      rank->names[rank->player_index][rank->mod_name_index] = c;
      setModNameIndex(rank, rank->mod_name_index + 1);
    }

    rank->redraw = 1;
  } else if (c == 'q')
    return -1;
  return 0;
}

void rankRender(Rank* rank, int milli) {
  if (rank->redraw == 1)
  {
      clear();
      for (int i=0;i<rank->length;i++)
      {
        move(2*(i+1), 2);
      	char text[NAME_LENGTH+5];
        char index_text[10];
        if (i == rank->player_index)
          sprintf(index_text, "%d -", i+1);
        else
          sprintf(index_text, "%d", i+1);
      	sprintf(text, "%s %s %d\n", index_text, rank->names[i], rank->score[i]);
      	addstr(text);
      }
      if (rank->player_index < 0) {
        if (rank->save == 0)
          addstr("you are not ranked (press q to exit)");
        else
          addstr("save success (press q to exit)");
      }
      else {
        addstr("type name and enter to save");
      }

      rank->redraw = 0;
  }
}
