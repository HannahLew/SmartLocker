bool passwordMatches()
{
    if(digitIndex != 4)
    {
        return false;
    }

    for(int i = 0; i < 4; i++)
    {
        if(enteredPassword[i] != savedPassword[i])
        {
            return false;
        }
    }

    return true;
}