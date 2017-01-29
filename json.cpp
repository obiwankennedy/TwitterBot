/***************************************************************************
    *   Copyright (C) 2016 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                                      *
    *                                                                         *
    *   rolisteam is free software; you can redistribute it and/or modify     *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    *                                                                         *
    *   This program is distributed in the hope that it will be useful,       *
    *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
    *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
    *   GNU General Public License for more details.                          *
    *                                                                         *
    *   You should have received a copy of the GNU General Public License     *
    *   along with this program; if not, write to the                         *
    *   Free Software Foundation, Inc.,                                       *
    *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
    ***************************************************************************/

#include "json.h"

UserList::UserList(std::string & data_in, int limit)
    : data(data_in) { //------------------------------------------------------------- ctor

    size_t last = this->data.find('[') + 1;
    size_t end = this->data.find(']');

    for(size_t next = this->data.find(',', last);
        limit > 0 && next < end && next != std::string::npos;
        next = this->data.find(',', last)){

        this->ids.push_back(this->data.substr(last, next - last));
        last = next + 1;
        --limit;
    }
}

UserList::UserList(std::string & data_in)
    : data(data_in) { //------------------------------------------------------------- ctor

    std::cout << this->data << std::endl;

    size_t last = this->data.find('[') + 1;
    size_t end = this->data.find(']');

    for(size_t next = this->data.find(',', last);
        next < end && next != std::string::npos;
        next = this->data.find(',', last)){

        this->ids.push_back(this->data.substr(last, next - last));
        last = next + 1;
    }
}

std::string & UserList::operator[](int i){ //--------------------------------------- operator[]
    return ids[i];
}

std::vector<std::string> & UserList::list(){ //------------------------------------- list
    return this->ids;
}

unsigned UserList::size(){ //------------------------------------------------------- size
    return ids.size();
}
User::User(std::string data_in) //--------------------------------------------------- ctor
    : data(data_in) {

    std::string meta, info;
    int first, last;
    int past = this->data.find("target");

    meta = "screen_name\":\"";
    first = this->data.find(meta,past) + meta.length();
    last = this->data.find('"',first);
    this->ship.name = this->data.substr(first, last - first);

    meta = "following\":";
    first = this->data.find(meta,past) + meta.length();
    last = this->data.find(',',first);
    info = this->data.substr(first, last - first);
    this->ship.following = (info == "true") ? true : false;

    meta = "followed_by\":";
    first = this->data.find(meta,past) + meta.length();
    last = this->data.find(',',first);
    info = this->data.substr(first, last - first);
    this->ship.follower = (info == "true") ? true : false;
}

Friendship & User::get_friendship(){ //---------------------------------------------- get_friendship
    return this->ship;
}

