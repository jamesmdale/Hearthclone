#include "Game\Entity\Character.hpp"
#include "Game\Entity\Player.hpp"

int Character::s_characterIndexer = 0;

Character::Character()
{
	m_characterId = GenerateNewCharacterId();
}


Character::~Character()
{

}

void Character::RefreshRenderables()
{
}

int Character::GenerateNewCharacterId()
{
	s_characterIndexer++;
	return s_characterIndexer;
}

void Character::ResetCharacterIndexer()
{
	s_characterIndexer = 0;
}

void Character::AddTag(const std::string& tagName)
{
	if(!CheckForTag(tagName))
		m_tags.push_back(tagName);
}

bool Character::CheckForTag(const std::string & tagName)
{
	for (int tagIndex = 0; tagIndex < (int)m_tags.size(); ++tagIndex)
	{
		if (m_tags[tagIndex] == tagName)
			return true;
	}

	return false;
}
