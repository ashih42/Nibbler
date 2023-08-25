#ifndef PLAYER_HPP
# define PLAYER_HPP

class Player
{
public:

	Player(void);
	~Player(void);

	int				getID(void) const;
	int				getWinCount(void) const;
	int				getScore(void) const;

	void			incrementWinCount(void);
	void			incrementScore(void);

private:

	static int		_numPlayers;

	int				_id;
	int				_winCount;
	int				_score;

	Player(const Player & src);
	Player & operator=(const Player & rhs);

};

#endif
