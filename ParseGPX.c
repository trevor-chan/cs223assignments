

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


/* states for the FSM:
 * 
 */
typedef enum {TRKPT,LAT,LON,ELE,TIME} state;

int main(int argc, char **argv)
{
	if (argc < 1)
	{
		printf("no file supplied");
		return 0;
	}
	else if (argc > 1)
	{
		printf("max 1 input file");
		return 0;
	}

	// the current state of the FSM
	state current = TRKPT;

	//current character from
	char ch;

	while (scanf("%c", &ch) != EOF)
	{
		switch (current)
		{
			case TRKPT:
				if (ch == '<')
				{
					ch = getchar();
					if (ch == 't' || ch == 'T')
					{
						ch = getchar();
						if (ch == 'r' || ch == 'R')
						{
							ch = getchar();
							if (ch == 'k' || ch == 'K')
							{
								ch = getchar();
								if (ch == 'p' || ch == 'P')
								{
									ch = getchar();
									if (ch == 't' || ch == 'T')
									{
										current = LAT;
									}
								}
							}
						}
					}
				}
				if (ch == EOF)
				{
					return 0;
				}
				break;

			case LAT:
				if (ch == '"')
				{
					ch = getchar();
					while (ch != '"' && ch != EOF)
					{
						ch = getchar();
					}
				}
				else if (ch == '\'')
				{
					ch = getchar();
					while (ch != '\'' && ch != EOF)
					{
						ch = getchar();
					}
				}
				while(isspace(ch) != 0)
				{
					ch = getchar();
				}
				if (ch == 'l')
				{
					ch = getchar();
					if (ch == 'a')
					{
						ch = getchar();
						if (ch == 't')
						{
							ch = getchar();
							while(isspace(ch) != 0)
							{
								ch = getchar();
							}
							if (ch == '=')
							{
								ch = getchar();
								while(isspace(ch) != 0)
								{
									ch = getchar();
								}
								if (ch == '"')
								{
									ch = getchar();
									while (ch != '"' && ch != EOF)
									{
										printf("%c",ch);
										ch = getchar();
									}
									printf(",");
									current = LON;
								}
								else if (ch == '\'')
								{
									ch = getchar();
									while (ch != '\'' && ch != EOF)
									{
										printf("%c",ch);
										ch = getchar();
									}
									printf(",");
									current = LON;
								}
							}
						}
					}
				}
				if (ch == EOF)
				{
					return 0;
				}
				break;

			case LON:
				if (ch == '"')
				{
					ch = getchar();
					while (ch != '"')
					{
						ch = getchar();
					}
				}
				else if (ch == '\'')
				{
					ch = getchar();
					while (ch != '"')
					{
						ch = getchar();
					}
				}
				while(isspace(ch) != 0)
				{
					ch = getchar();
				}
				if (ch == 'l')
				{
					ch = getchar();
					if (ch == 'o')
					{
						ch = getchar();
						if (ch == 'n')
						{
							ch = getchar();
							while(isspace(ch) != 0)
							{
								ch = getchar();
							}
							if (ch == '=')
							{
								ch = getchar();
								while(isspace(ch) != 0)
								{
									ch = getchar();
								}
								if (ch == '"')
								{
									ch = getchar();
									while (ch != '"' && ch != EOF)
									{
										printf("%c",ch);
										ch = getchar();
									}
									printf(",");
									current = ELE;
								}
								else if (ch == '\'')
								{
									ch = getchar();
									while (ch != '\'' && ch != EOF)
									{
										printf("%c",ch);
										ch = getchar();
									}
									printf(",");
									current = ELE;
								}
							}
						}
					}
				}
				if (ch == EOF)
				{
					return 0;
				}
				break;

			case ELE:
				if (ch == '<')
				{
					ch = getchar();
					if (ch == 'e' || ch == 'E')
					{
						ch = getchar();
						if (ch == 'l' || ch == 'L')
						{
							ch = getchar();
							if (ch == 'e' || ch == 'E')
							{
								ch = getchar();
								while(isspace(ch) != 0)
								{
									ch = getchar();
								}
								while(ch != '>' && ch != EOF)
								{
									ch = getchar();
									if(ch == EOF)
									{
										return 0;
									}
								}
									ch = getchar();
									while (ch != '<' && ch != EOF)
									{
										printf("%c",ch);
										ch = getchar();
									}
									printf(",");
									current = TIME;
							}
						}
					}
				}
				if (ch == EOF)
				{
					return 0;
				}
				break;

			case TIME:
				if (ch == '<')
				{
					ch = getchar();
					if (ch == 't' || ch == 'T')
					{
						ch = getchar();
						if (ch == 'i' || ch == 'I')
						{
							ch = getchar();
							if (ch == 'm' || ch == 'M')
							{
								ch = getchar();
								if (ch == 'e' || ch == 'E')
								{
									ch = getchar();
									while(isspace(ch) != 0)
									{
										ch = getchar();
									}
									if (ch == '>')
									{
										ch = getchar();
										while (ch != '<' && ch != EOF)
										{
											if (ch == ',')
											{
												printf("&comma;");
											}
											else
											{
												printf("%c",ch);
											}
											ch = getchar();
										}
										printf("\n");											
										current = TRKPT;
									}
								}
							}
						}
					}
				}
				if (ch == EOF)
				{
					return 0;
				}
				break;
		}
	}
	return 0;
}