/* SavedGame.cpp
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "SavedGame.h"

#include "DataFile.h"
#include "DataNode.h"
#include "Date.h"
#include "text/Format.h"
#include "GameData.h"
#include "Planet.h"
#include "image/SpriteSet.h"
#include "System.h"

using namespace std;



SavedGame::SavedGame(const filesystem::path &path)
{
	Load(path);
}



void SavedGame::Load(const filesystem::path &path)
{
	Clear();
	DataFile file(path);
	if(file.begin() != file.end())
		this->path = path;

	int flagshipIterator = -1;
	int flagshipTarget = 0;

	for(const DataNode &node : file)
	{
		if(node.Token(0) == "pilot" && node.Size() >= 3)
			name = node.Token(1) + " " + node.Token(2);
		else if(node.Token(0) == "date" && node.Size() >= 4)
			date = Date(node.Value(1), node.Value(2), node.Value(3)).ToString();
		else if(node.Token(0) == "system" && node.Size() >= 2)
		{
			system = node.Token(1);
			const System *savedSystem = GameData::Systems().Find(system);
			if(savedSystem && savedSystem->IsValid())
				system = savedSystem->DisplayName();
		}
		else if(node.Token(0) == "planet" && node.Size() >= 2)
		{
			planet = node.Token(1);
			const Planet *savedPlanet = GameData::Planets().Find(planet);
			if(savedPlanet && savedPlanet->IsValid())
				planet = savedPlanet->DisplayName();
		}
		else if(node.Token(0) == "playtime" && node.Size() >= 2)
			playTime = Format::PlayTime(node.Value(1));
		else if(node.Token(0) == "flagship index" && node.Size() >= 2)
			flagshipTarget = node.Value(1);
		else if(node.Token(0) == "account")
		{
			for(const DataNode &child : node)
				if(child.Token(0) == "credits" && child.Size() >= 2)
				{
					credits = Format::Credits(child.Value(1));
					break;
				}
		}
		else if(node.Token(0) == "ship" && ++flagshipIterator == flagshipTarget)
		{
			for(const DataNode &child : node)
			{
				if(child.Token(0) == "name" && child.Size() >= 2)
					shipName = child.Token(1);
				else if(child.Token(0) == "sprite" && child.Size() >= 2)
					shipSprite = SpriteSet::Get(child.Token(1));
			}
		}
	}
}



const filesystem::path &SavedGame::Path() const
{
	return path;
}



bool SavedGame::IsLoaded() const
{
	return !path.empty();
}



void SavedGame::Clear()
{
	path.clear();

	name.clear();
	credits.clear();
	date.clear();

	system.clear();
	planet.clear();
	playTime = "0s";

	shipSprite = nullptr;
	shipName.clear();
}



const string &SavedGame::Name() const
{
	return name;
}



const string &SavedGame::Credits() const
{
	return credits;
}



const string &SavedGame::GetDate() const
{
	return date;
}



const string &SavedGame::GetSystem() const
{
	return system;
}



const string &SavedGame::GetPlanet() const
{
	return planet;
}



const string &SavedGame::GetPlayTime() const
{
	return playTime;
}



const Sprite *SavedGame::ShipSprite() const
{
	return shipSprite;
}



const string &SavedGame::ShipName() const
{
	return shipName;
}
