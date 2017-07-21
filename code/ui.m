function varargout = ui(varargin)
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @ui_OpeningFcn, ...
                   'gui_OutputFcn',  @ui_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end

% --- Executes just before ui is made visible.
function ui_OpeningFcn(hObject, eventdata, handles, varargin)
global meetingstarted serialObject
warning off MATLAB:serial:fread:unsuccessfulRead
handles.output = hObject;
delete(instrfindall);
serialPort = 'COM5';
serialObject = serial(serialPort, 'BaudRate', 115200);
set(serialObject,'Timeout',1);
fopen(serialObject);
handles.name1registered = 0;
handles.name2registered = 0;
handles.registering = 0;
meetingstarted = 0;

% Update handles structure
guidata(hObject, handles);


% --- Outputs from this function are returned to the command line.
function varargout = ui_OutputFcn(hObject, eventdata, handles) 
varargout{1} = handles.output;



function name1_Callback(hObject, eventdata, handles)

% --- Executes during object creation, after setting all properties.
function name1_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in register1.
function register1_Callback(hObject, eventdata, handles)
global serialObject
disp('Registering name 1');
fwrite(serialObject, 1, 'uint8');


function name2_Callback(hObject, eventdata, handles)

% --- Executes during object creation, after setting all properties.
function name2_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in register2.
function register2_Callback(hObject, eventdata, handles)
global serialObject
disp('Registering name 2');
fwrite(serialObject, 2, 'uint8');

function timerCallback(hTimer, eventData, hObject, handles)
global serialObject
try
    warning off MATLAB:serial:fread:unsuccessfulRead
    temp = uint8(fread(serialObject, 1, 'uint8'));
    if (temp == 111)
        set(handles.StatusDisplay, 'string', strcat(get(handles.name1,'string'), ' is speaking'));
        guidata(hObject);
    elseif (temp == 112)
        set(handles.StatusDisplay, 'string', strcat(get(handles.name2,'string'), ' is speaking'));      
        guidata(hObject);
    elseif (temp == 113)
        set(handles.StatusDisplay, 'string', strcat(get(handles.name3,'string'), ' is speaking'));      
        guidata(hObject);
    elseif (temp == 114)
        set(handles.StatusDisplay, 'string', strcat(get(handles.name4,'string'), ' is speaking'));      
        guidata(hObject);
    elseif (temp == 100)
        set(handles.StatusDisplay, 'string', 'Stranger is speaking');
        guidata(hObject);
    end
catch exception
end

% --- Executes on button press in startmeeting.
function startmeeting_Callback(hObject, eventdata, handles)
global meetingstarted serialObject tmr
if meetingstarted == 0
    disp('Start meeting');
    meetingstarted = 1;
    set(handles.startmeeting,'string','End Meeting');
    set(handles.StatusDisplay,'string','');
    set(handles.register1,'Enable','off');
    set(handles.register2,'Enable','off');
    set(handles.name1,'Enable','off');
    set(handles.name2,'Enable','off');
    fwrite(serialObject, 5, 'uint8');
    guidata(hObject, handles);
    tmr = timer('TimerFcn',{@timerCallback,hObject,handles},'Period', 0.1,'ExecutionMode','fixedSpacing');
    start(tmr);
else
    disp('End meeting');
    meetingstarted = 0;
    stop(tmr);
    set(handles.startmeeting,'string','Start Meeting');
    set(handles.StatusDisplay,'string','Meeting has not started.');
    set(handles.register1,'Enable','on');
    set(handles.register2,'Enable','on');
    set(handles.name1,'Enable','on');
    set(handles.name2,'Enable','on');
    guidata(hObject, handles);
end



function name3_Callback(hObject, eventdata, handles)


% --- Executes during object creation, after setting all properties.
function name3_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function name4_Callback(hObject, eventdata, handles)


% --- Executes during object creation, after setting all properties.
function name4_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in pushbutton4.
function pushbutton4_Callback(hObject, eventdata, handles)


% --- Executes on button press in pushbutton5.
function pushbutton5_Callback(hObject, eventdata, handles)
