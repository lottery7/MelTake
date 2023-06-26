<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <class>MainWindow</class>
 <widget class="QMainWindow" name="MainWindow">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>1400</width>
    <height>900</height>
   </rect>
  </property>
  <property name="palette">
   <palette>
    <active>
     <colorrole role="Window">
      <brush brushstyle="SolidPattern">
       <color alpha="255">
        <red>180</red>
        <green>200</green>
        <blue>193</blue>
       </color>
      </brush>
     </colorrole>
    </active>
    <inactive>
     <colorrole role="Window">
      <brush brushstyle="SolidPattern">
       <color alpha="255">
        <red>240</red>
        <green>240</green>
        <blue>240</blue>
       </color>
      </brush>
     </colorrole>
    </inactive>
    <disabled>
     <colorrole role="Window">
      <brush brushstyle="SolidPattern">
       <color alpha="255">
        <red>180</red>
        <green>200</green>
        <blue>193</blue>
       </color>
      </brush>
     </colorrole>
    </disabled>
   </palette>
  </property>
  <property name="windowTitle">
   <string>MainWindow</string>
  </property>
  <property name="styleSheet">
   <string notr="true">#main_layout{

	background-color: 
			qlineargradient(
					spread:pad, x1:0, y1:1, x2:0, y2:0, 
					stop:0.0 rgba(238,193,117, 255),
					stop:0.33 rgba(238,237,176, 255),
					stop:0.66 rgba(218,238,222, 255), 
					stop:1.0 rgba(179,219,192, 255)
			);

}
</string>
  </property>
  <widget class="QWidget" name="main_layout">
   <layout class="QHBoxLayout" name="horizontalLayout_3">
    <item>
     <layout class="QVBoxLayout" name="playlists_layout_2">
      <property name="sizeConstraint">
       <enum>QLayout::SetDefaultConstraint</enum>
      </property>
      <item>
       <layout class="QHBoxLayout" name="horizontalLayout_4">
        <item>
         <widget class="QPushButton" name="settings_button">
          <property name="styleSheet">
           <string notr="true">QPushButton{
	color: rgb(45,75,55);
	background-color: rgba(238,193,117,220);
selection-color: rgb(45,75,55);
	selection-border: 0px;
	selection-background-color: rgba(179,219,192,220);
	border: 1px solid rgba(215,125,40,200);
	border-radius: 3px;
	font-weight: bold;
    font-size: 25px;
	padding: 2px 4px;
}
</string>
          </property>
          <property name="text">
           <string>Settings</string>
          </property>
         </widget>
        </item>
        <item>
         <widget class="QPushButton" name="load_tracks_button">
          <property name="styleSheet">
           <string notr="true">QPushButton{
	color: rgb(45,75,55);
	background-color: rgba(238,193,117,220);
selection-color: rgb(45,75,55);
	selection-border: 0px;
	selection-background-color: rgba(179,219,192,220);
	border: 1px solid rgba(215,125,40,200);
	border-radius: 3px;
	font-weight: bold;
    font-size: 25px;
	padding: 2px 4px;
}
</string>
          </property>
          <property name="text">
           <string>Load new tracks</string>
          </property>
          <property name="checkable">
           <bool>true</bool>
          </property>
          <property name="checked">
           <bool>false</bool>
          </property>
         </widget>
        </item>
        <item>
         <spacer name="horizontalSpacer_5">
          <property name="orientation">
           <enum>Qt::Horizontal</enum>
          </property>
          <property name="sizeHint" stdset="0">
           <size>
            <width>40</width>
            <height>20</height>
           </size>
          </property>
         </spacer>
        </item>
       </layout>
      </item>
      <item>
       <layout class="QHBoxLayout" name="horizontalLayout_5">
        <item>
         <widget class="QLabel" name="album_cover_lable">
          <property name="enabled">
           <bool>true</bool>
          </property>
          <property name="sizePolicy">
           <sizepolicy hsizetype="Fixed" vsizetype="Fixed">
            <horstretch>0</horstretch>
            <verstretch>0</verstretch>
           </sizepolicy>
          </property>
          <property name="text">
           <string>Standart_app_cover</string>
          </property>
          <property name="scaledContents">
           <bool>true</bool>
          </property>
         </widget>
        </item>
        <item>
         <layout class="QGridLayout" name="gridLayout_2">
          <item row="0" column="1">
           <spacer name="horizontalSpacer_3">
            <property name="orientation">
             <enum>Qt::Horizontal</enum>
            </property>
            <property name="sizeHint" stdset="0">
             <size>
              <width>40</width>
              <height>20</height>
             </size>
            </property>
           </spacer>
          </item>
          <item row="0" column="0">
           <widget class="QLabel" name="track_title_lable">
            <property name="sizePolicy">
             <sizepolicy hsizetype="Fixed" vsizetype="Minimum">
              <horstretch>0</horstretch>
              <verstretch>0</verstretch>
             </sizepolicy>
            </property>
            <property name="minimumSize">
             <size>
              <width>600</width>
              <height>0</height>
             </size>
            </property>
            <property name="maximumSize">
             <size>
              <width>600</width>
              <height>16777215</height>
             </size>
            </property>
            <property name="font">
             <font>
              <pointsize>11</pointsize>
              <bold>true</bold>
             </font>
            </property>
            <property name="styleSheet">
             <string notr="true">QLabel
{
	color: rgb(45,75,55);
}</string>
            </property>
            <property name="text">
             <string>Track_Title</string>
            </property>
           </widget>
          </item>
          <item row="3" column="0">
           <layout class="QHBoxLayout" name="track_control_panel_layout">
            <property name="spacing">
             <number>10</number>
            </property>
            <property name="sizeConstraint">
             <enum>QLayout::SetMinimumSize</enum>
            </property>
            <item>
             <layout class="QGridLayout" name="play_panel">
              <property name="sizeConstraint">
               <enum>QLayout::SetMaximumSize</enum>
              </property>
              <property name="verticalSpacing">
               <number>0</number>
              </property>
              <item row="1" column="0">
               <widget class="QPushButton" name="previous_track_button">
                <property name="sizePolicy">
                 <sizepolicy hsizetype="Fixed" vsizetype="Fixed">
                  <horstretch>0</horstretch>
                  <verstretch>0</verstretch>
                 </sizepolicy>
                </property>
                <property name="styleSheet">
                 <string notr="true">QPushButton {
border-image: url(:/resources/buttons/previous.png);
background: none;
width: 30px;
height: 30px;
}</string>
                </property>
                <property name="text">
                 <string/>
                </property>
               </widget>
              </item>
              <item row="1" column="1">
               <spacer name="horizontalSpacer">
                <property name="orientation">
                 <enum>Qt::Horizontal</enum>
                </property>
                <property name="sizeType">
                 <enum>QSizePolicy::Fixed</enum>
                </property>
                <property name="sizeHint" stdset="0">
                 <size>
                  <width>40</width>
                  <height>20</height>
                 </size>
                </property>
               </spacer>
              </item>
              <item row="0" column="1">
               <widget class="QPushButton" name="play_pause_button">
                <property name="enabled">
                 <bool>true</bool>
                </property>
                <property name="sizePolicy">
                 <sizepolicy hsizetype="Fixed" vsizetype="Fixed">
                  <horstretch>0</horstretch>
                  <verstretch>0</verstretch>
                 </sizepolicy>
                </property>
                <property name="styleSheet">
                 <string notr="true">QPushButton {
border-image: url(:/resources/buttons/play.png);
background: none;
width: 40px;
height: 40px;
}

QPushButton:checked
{
   border-image: url(:/resources/buttons/pause.png);
}

</string>
                </property>
                <property name="text">
                 <string/>
                </property>
                <property name="checkable">
                 <bool>true</bool>
                </property>
                <property name="checked">
                 <bool>false</bool>
                </property>
               </widget>
              </item>
              <item row="1" column="2">
               <widget class="QPushButton" name="next_track_button">
                <property name="sizePolicy">
                 <sizepolicy hsizetype="Fixed" vsizetype="Fixed">
                  <horstretch>0</horstretch>
                  <verstretch>0</verstretch>
                 </sizepolicy>
                </property>
                <property name="styleSheet">
                 <string notr="true">QPushButton {
border-image: url(:/resources/buttons/next.png);
background: none;
width: 30px;
height: 30px;
}</string>
                </property>
                <property name="text">
                 <string/>
                </property>
               </widget>
              </item>
             </layout>
            </item>
            <item>
             <layout class="QGridLayout" name="gridLayout">
              <property name="sizeConstraint">
               <enum>QLayout::SetDefaultConstraint</enum>
              </property>
              <item row="1" column="0">
               <widget class="QPushButton" name="track_loop_mode_button">
                <property name="sizePolicy">
                 <sizepolicy hsizetype="Fixed" vsizetype="Fixed">
                  <horstretch>0</horstretch>
                  <verstretch>0</verstretch>
                 </sizepolicy>
                </property>
                <property name="styleSheet">
                 <string notr="true">QPushButton {
border-image: url(:/resources/buttons/loop.png);
background: none;
width: 30px;
height: 30px;
}</string>
                </property>
                <property name="text">
                 <string/>
                </property>
                <property name="checkable">
                 <bool>true</bool>
                </property>
               </widget>
              </item>
              <item row="2" column="1">
               <widget class="QSlider" name="volume_slider">
                <property name="sizePolicy">
                 <sizepolicy hsizetype="Fixed" vsizetype="Fixed">
                  <horstretch>0</horstretch>
                  <verstretch>0</verstretch>
                 </sizepolicy>
                </property>
                <property name="styleSheet">
                 <string notr="true">QSlider::groove:horizontal {
	height: 14px;
	background: rgba(132,184,162,255);
	bourder: 2px gba(132,184,162,255);
	broder-radius: 8px;
}

QSlider::handle:horizontal {
	background: rgba(103,146,144,255);
	border: 4px solid gba(103,146,144,255);
	width:10px;
	height:10px;
	margin: -5px 0;
border-radius: 3px;
		
}

QSlider::sub-page:horizontal {
    background: rgba(238,193,117,255);
}

</string>
                </property>
                <property name="value">
                 <number>60</number>
                </property>
                <property name="orientation">
                 <enum>Qt::Horizontal</enum>
                </property>
               </widget>
              </item>
              <item row="0" column="0">
               <widget class="QPushButton" name="random_mode_button">
                <property name="sizePolicy">
                 <sizepolicy hsizetype="Fixed" vsizetype="Fixed">
                  <horstretch>0</horstretch>
                  <verstretch>0</verstretch>
                 </sizepolicy>
                </property>
                <property name="styleSheet">
                 <string notr="true">QPushButton {
border-image: url(:/resources/buttons/random.png);
background: none;
width: 30px;
height: 30px;
}</string>
                </property>
                <property name="text">
                 <string/>
                </property>
                <property name="checkable">
                 <bool>true</bool>
                </property>
               </widget>
              </item>
              <item row="2" column="0">
               <widget class="QPushButton" name="mute_button">
                <property name="sizePolicy">
                 <sizepolicy hsizetype="Fixed" vsizetype="Fixed">
                  <horstretch>0</horstretch>
                  <verstretch>0</verstretch>
                 </sizepolicy>
                </property>
                <property name="styleSheet">
                 <string notr="true">QPushButton {
	border-image: url(:/resources/buttons/volume_on.png);
	background: none;

}

QPushButton:checked
{
   border-image: url(:/resources/buttons/volume_off.png);
}</string>
                </property>
                <property name="text">
                 <string/>
                </property>
                <property name="checkable">
                 <bool>true</bool>
                </property>
               </widget>
              </item>
             </layout>
            </item>
           </layout>
          </item>
          <item row="1" column="0">
           <widget class="QLabel" name="track_artist_album_lable">
            <property name="sizePolicy">
             <sizepolicy hsizetype="Fixed" vsizetype="Minimum">
              <horstretch>0</horstretch>
              <verstretch>0</verstretch>
             </sizepolicy>
            </property>
            <property name="minimumSize">
             <size>
              <width>600</width>
              <height>0</height>
             </size>
            </property>
            <property name="maximumSize">
             <size>
              <width>600</width>
              <height>16777215</height>
             </size>
            </property>
            <property name="styleSheet">
             <string notr="true">QLabel
{
	color: rgb(45,75,55);
}</string>
            </property>
            <property name="text">
             <string>Track_ Artist - Track_Album</string>
            </property>
           </widget>
          </item>
          <item row="2" column="0">
           <spacer name="verticalSpacer">
            <property name="orientation">
             <enum>Qt::Vertical</enum>
            </property>
            <property name="sizeType">
             <enum>QSizePolicy::Fixed</enum>
            </property>
            <property name="sizeHint" stdset="0">
             <size>
              <width>20</width>
              <height>40</height>
             </size>
            </property>
           </spacer>
          </item>
         </layout>
        </item>
       </layout>
      </item>
      <item>
       <layout class="QHBoxLayout" name="horizontalLayout_2">
        <item>
         <widget class="QSlider" name="process_slyder">
          <property name="styleSheet">
           <string notr="true">QSlider::groove:horizontal {
	height: 14px;
	background: rgba(132,184,162,255);
	bourder: 2px gba(132,184,162,255);
	broder-radius: 8px;
}

QSlider::handle:horizontal {
	background: rgba(103,146,144,255);
	border: 4px solid gba(103,146,144,255);
	width:10px;
	height:10px;
	margin: -5px 0;
border-radius: 3px;
		
}

QSlider::sub-page:horizontal {
    background: rgba(238,193,117,255);
}

</string>
          </property>
          <property name="singleStep">
           <number>20</number>
          </property>
          <property name="value">
           <number>0</number>
          </property>
          <property name="sliderPosition">
           <number>0</number>
          </property>
          <property name="orientation">
           <enum>Qt::Horizontal</enum>
          </property>
          <property name="invertedAppearance">
           <bool>false</bool>
          </property>
          <property name="tickPosition">
           <enum>QSlider::TicksBothSides</enum>
          </property>
          <property name="tickInterval">
           <number>1</number>
          </property>
         </widget>
        </item>
        <item>
         <widget class="QLabel" name="track_time_lable">
          <property name="sizePolicy">
           <sizepolicy hsizetype="Fixed" vsizetype="Fixed">
            <horstretch>0</horstretch>
            <verstretch>0</verstretch>
           </sizepolicy>
          </property>
          <property name="text">
           <string>j00:00</string>
          </property>
         </widget>
        </item>
       </layout>
      </item>
      <item>
       <layout class="QHBoxLayout" name="horizontalLayout">
        <item>
         <spacer name="horizontalSpacer_2">
          <property name="orientation">
           <enum>Qt::Horizontal</enum>
          </property>
          <property name="sizeHint" stdset="0">
           <size>
            <width>40</width>
            <height>20</height>
           </size>
          </property>
         </spacer>
        </item>
        <item>
         <widget class="QLabel" name="current_playlist_tabel">
          <property name="sizePolicy">
           <sizepolicy hsizetype="Minimum" vsizetype="Minimum">
            <horstretch>0</horstretch>
            <verstretch>0</verstretch>
           </sizepolicy>
          </property>
          <property name="font">
           <font>
            <family>Segoe UI</family>
            <pointsize>15</pointsize>
            <bold>true</bold>
           </font>
          </property>
          <property name="styleSheet">
           <string notr="true">QLabel
{
   	color: rgb(45,75,55);
}
</string>
          </property>
          <property name="text">
           <string>Playlist Name</string>
          </property>
         </widget>
        </item>
        <item>
         <spacer name="horizontalSpacer_6">
          <property name="orientation">
           <enum>Qt::Horizontal</enum>
          </property>
          <property name="sizeHint" stdset="0">
           <size>
            <width>40</width>
            <height>20</height>
           </size>
          </property>
         </spacer>
        </item>
       </layout>
      </item>
      <item>
       <widget class="QTableView" name="tracks_tabel_view">
        <property name="tabletTracking">
         <bool>false</bool>
        </property>
        <property name="contextMenuPolicy">
         <enum>Qt::NoContextMenu</enum>
        </property>
        <property name="layoutDirection">
         <enum>Qt::LeftToRight</enum>
        </property>
        <property name="styleSheet">
         <string notr="true">QTableView {
	color: rgb(45,75,55);
	background-color: rgba(238,193,117,100);
	alternate-background-color: rgba(215,125,40,40);
	selection-color: rgb(45,75,55);
	selection-border: 0px;
	selection-background-color: rgba(179,219,192,200);
	border: 3px solid rgba(215,125,40,100);
	border-radius: 10px;

	font-weight: bold;
    font-size: 25px;
}</string>
        </property>
        <property name="frameShape">
         <enum>QFrame::Panel</enum>
        </property>
        <property name="frameShadow">
         <enum>QFrame::Plain</enum>
        </property>
        <property name="lineWidth">
         <number>10</number>
        </property>
        <property name="midLineWidth">
         <number>10</number>
        </property>
        <property name="verticalScrollBarPolicy">
         <enum>Qt::ScrollBarAlwaysOn</enum>
        </property>
        <property name="horizontalScrollBarPolicy">
         <enum>Qt::ScrollBarAlwaysOff</enum>
        </property>
        <property name="alternatingRowColors">
         <bool>true</bool>
        </property>
        <property name="showGrid">
         <bool>false</bool>
        </property>
        <property name="gridStyle">
         <enum>Qt::NoPen</enum>
        </property>
        <property name="sortingEnabled">
         <bool>false</bool>
        </property>
        <property name="wordWrap">
         <bool>false</bool>
        </property>
        <property name="cornerButtonEnabled">
         <bool>false</bool>
        </property>
        <attribute name="horizontalHeaderVisible">
         <bool>false</bool>
        </attribute>
        <attribute name="horizontalHeaderMinimumSectionSize">
         <number>50</number>
        </attribute>
        <attribute name="horizontalHeaderDefaultSectionSize">
         <number>50</number>
        </attribute>
        <attribute name="horizontalHeaderHighlightSections">
         <bool>false</bool>
        </attribute>
        <attribute name="verticalHeaderHighlightSections">
         <bool>false</bool>
        </attribute>
       </widget>
      </item>
     </layout>
    </item>
    <item>
     <layout class="QVBoxLayout" name="verticalLayout">
      <item>
       <layout class="QHBoxLayout" name="horizontalLayout_6">
        <property name="sizeConstraint">
         <enum>QLayout::SetMaximumSize</enum>
        </property>
        <item>
         <widget class="QPushButton" name="add_playlist">
          <property name="styleSheet">
           <string notr="true">QPushButton{
	color: rgb(45,75,55);
	background-color: rgba(238,193,117,220);
selection-color: rgb(45,75,55);
	selection-border: 0px;
	selection-background-color: rgba(179,219,192,220);
	border: 1px solid rgba(215,125,40,200);
	border-radius: 3px;
	font-weight: bold;
    font-size: 25px;
	padding: 2px 4px;
}
</string>
          </property>
          <property name="text">
           <string>Add new playlist</string>
          </property>
         </widget>
        </item>
        <item>
         <spacer name="horizontalSpacer_4">
          <property name="orientation">
           <enum>Qt::Horizontal</enum>
          </property>
          <property name="sizeType">
           <enum>QSizePolicy::Expanding</enum>
          </property>
          <property name="sizeHint" stdset="0">
           <size>
            <width>113</width>
            <height>20</height>
           </size>
          </property>
         </spacer>
        </item>
       </layout>
      </item>
      <item>
       <widget class="QTableView" name="playlists_table_view">
        <property name="sizePolicy">
         <sizepolicy hsizetype="MinimumExpanding" vsizetype="Expanding">
          <horstretch>200</horstretch>
          <verstretch>0</verstretch>
         </sizepolicy>
        </property>
        <property name="minimumSize">
         <size>
          <width>400</width>
          <height>0</height>
         </size>
        </property>
        <property name="styleSheet">
         <string notr="true">QTableView {
	color: rgb(45,75,55);
	background-color: rgba(238,193,117,100);
	alternate-background-color: rgba(215,125,40,40);
	selection-color: rgb(45,75,55);
	selection-border: 0px;
	selection-background-color: rgba(179,219,192,200);
	border: 3px solid rgba(215,125,40,100);
	border-radius: 10px;

	font-weight: bold;
    font-size: 25px;
}</string>
        </property>
       </widget>
      </item>
     </layout>
    </item>
   </layout>
  </widget>
 </widget>
 <resources/>
 <connections/>
</ui>
