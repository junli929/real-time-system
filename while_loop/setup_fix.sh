#!/bin/bash

# -- Config --
echo "Reading configuration..."
source config

LAB_GROUP_TEAM=1

WIDTH=140
PH_RADIUS=140
EV_RADIUS=300
EV_DIST=400
SEC_MAX_SIZE=200

# -- Run Setup --
echo "Setup in progress..."

# robot agent: configure robot and group port
sed -i "s/\(id *= *\).*/\1$ROBOT_ID/" ./robot_agent/bin/res/config.ini
sed -i "s/\(port *= *\).*/\1$LAB_GROUP_PORT/" ./robot_agent/bin/res/config.ini
sed -i "s/\(team*= *\).*/\1$LAB_GROUP_TEAM/" ./robot_agent/bin/res/config.ini

sed -i "s/\(id *= *\).*/\1$ROBOT_ID/" ./robot_agent/res/config.ini
sed -i "s/\(port *= *\).*/\1$LAB_GROUP_PORT/" ./robot_agent/res/config.ini
sed -i "s/\(team *= *\).*/\1$LAB_GROUP_TEAM/" ./robot_agent/res/config.ini

# configure pheromone map parameters
sed -i "s/\(width *= *\).*/\1$WIDTH/" ./robot_agent/bin/res/config.ini
sed -i "s/\(pheromone_radius  *= *\).*/\1$PH_RADIUS/" ./robot_agent/bin/res/config.ini
sed -i "s/\(eval_radius *= *\).*/\1$EV_RADIUS/" ./robot_agent/bin/res/config.ini
sed -i "s/\(eval_dist  *= *\).*/\1$EV_DIST/" ./robot_agent/bin/res/config.ini
sed -i "s/\(sector_max_size *= *\).*/\1$SEC_MAX_SIZE/" ./robot_agent/bin/res/config.ini

sed -i "s/\(width *= *\).*/\1$WIDTH/" ./robot_agent/res/config.ini
sed -i "s/\(pheromone_radius  *= *\).*/\1$PH_RADIUS/" ./robot_agent/res/config.ini
sed -i "s/\(eval_radius *= *\).*/\1$EV_RADIUS/" ./robot_agent/res/config.ini
sed -i "s/\(eval_dist  *= *\).*/\1$EV_DIST/" ./robot_agent/res/config.ini
sed -i "s/\(sector_max_size *= *\).*/\1$SEC_MAX_SIZE/" ./robot_agent/res/config.ini

echo "Fix done!"
