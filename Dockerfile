FROM python:3.7

ENV APP_VERSION="4.3.1" \
    APP="platformio"

LABEL app.name="${APP}" \
      app.version="${APP_VERSION}" \
      maintainer="Marco Massarelli <marco.massarelli@gmail.com>"

RUN pip install -U ${APP}==${APP_VERSION} && \
    mkdir -p /workspace

WORKDIR /workspace

ENTRYPOINT ["platformio"]
