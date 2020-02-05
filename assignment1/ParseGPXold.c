

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


/* states for the FSM:
 * 
 */
typedef enum {TRKPT,LAT,LON,ELE,TIME} state;

int main(char **argv)
{
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
					scanf("%c", &ch);
					if (ch == 't' || ch == 'T')
					{
						scanf("%c", &ch);
						if (ch == 'r' || ch == 'R')
						{
							scanf("%c", &ch);
							if (ch == 'k' || ch == 'K')
							{
								scanf("%c", &ch);
								if (ch == 'p' || ch == 'P')
								{
									scanf("%c", &ch);
									if (ch == 't' || ch == 'T')
									{
										current = LAT;
										//printf("\nswitch to LAT\n");
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
					//scanf("%c", &ch);
					ch = getchar();
					while (ch != '"' && ch != EOF)
					{
						scanf("%c", &ch);
					}
				}
				else if (ch == '\'')
				{
					scanf("%c", &ch);
					while (ch != '\'' && ch != EOF)
					{
						scanf("%c", &ch);
					}
				}
				while(isspace(ch) != 0)
				{
					scanf("%c", &ch);
				}
				if (ch == 'l')
				{
					scanf("%c", &ch);
					if (ch == 'a')
					{
						scanf("%c", &ch);
						if (ch == 't')
						{
							scanf("%c", &ch);
							while(isspace(ch) != 0)
							{
								scanf("%c", &ch);
							}
							if (ch == '=')
							{
								scanf("%c", &ch);
								while(isspace(ch) != 0)
								{
									scanf("%c", &ch);
								}
								if (ch == '"')
								{
									scanf("%c", &ch);
									while (ch != '"' && ch != EOF)
									{
										printf("%c",ch);
										scanf("%c", &ch);
										if (ch == EOF)////////////////////////////------------
										{
											printf("end");
											return 0;
										}
									}
									printf(",");
									current = LON;
								}
								else if (ch == '\'')
								{
									scanf("%c", &ch);
									while (ch != '\'' && ch != EOF)
									{
										printf("%c",ch);
										scanf("%c", &ch);
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
					scanf("%c", &ch);
					while (ch != '"')
					{
						scanf("%c", &ch);
					}
				}
				else if (ch == '\'')
				{
					scanf("%c", &ch);
					while (ch != '"')
					{
						scanf("%c", &ch);
					}
				}
				while(isspace(ch) != 0)
				{
					scanf("%c", &ch);
				}
				if (ch == 'l')
				{
					scanf("%c", &ch);
					if (ch == 'o')
					{
						scanf("%c", &ch);
						if (ch == 'n')
						{
							scanf("%c", &ch);
							while(isspace(ch) != 0)
							{
								scanf("%c", &ch);
							}
							if (ch == '=')
							{
								scanf("%c", &ch);
								while(isspace(ch) != 0)
								{
									scanf("%c", &ch);
								}
								if (ch == '"')
								{
									scanf("%c", &ch);
									while (ch != '"' && ch != EOF)
									{
										printf("%c",ch);
										scanf("%c", &ch);
									}
									printf(",");
									current = ELE;
									//printf("\nswitch to ELE\n");
								}
								else if (ch == '\'')
								{
									scanf("%c", &ch);
									while (ch != '"' && ch != EOF)
									{
										printf("%c",ch);
										scanf("%c", &ch);
									}
									printf(",");
									current = ELE;
									//printf("\nswitch to ELE\n");
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
					scanf("%c", &ch);
					if (ch == 'e' || ch == 'E')
					{
						scanf("%c", &ch);
						if (ch == 'l' || ch == 'L')
						{
							scanf("%c", &ch);
							if (ch == 'e' || ch == 'E')
							{
								scanf("%c", &ch);
								while(isspace(ch) != 0)
								{
									scanf("%c", &ch);
								}
								while(ch != '>' && ch != EOF)
								{
									scanf("%c",&ch);
									if(ch == EOF)
									{
										return 0;
									}
								}
								//if (ch == '>')
								//{
									scanf("%c",&ch);
									while (ch != '<' && ch != EOF)
									{
										printf("%c",ch);
										scanf("%c",&ch);
									}
									printf(",");
									current = TIME;
									//printf("\nswitch to TIME\n");
								//}
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
					scanf("%c", &ch);
					if (ch == 't' || ch == 'T')
					{
						scanf("%c", &ch);
						if (ch == 'i' || ch == 'I')
						{
							scanf("%c", &ch);
							if (ch == 'm' || ch == 'M')
							{
								scanf("%c", &ch);
								if (ch == 'e' || ch == 'E')
								{
									scanf("%c", &ch);
									while(isspace(ch) != 0)
									{
										scanf("%c", &ch);
									}
									if (ch == '>')
									{
										scanf("%c",&ch);
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
											scanf("%c",&ch);
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