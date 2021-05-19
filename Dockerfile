FROM python:latest

COPY course ./
COPY ev3 ./
COPY simulator2d ./
COPY workspace ./

RUN pip3 install -r simulator2d/requirements-sim.txt
RUN cp workspace/config-sim.ini workspace/config.ini

CMD ["python3", "simulator2d/main.py"]
