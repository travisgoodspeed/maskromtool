FROM debian:trixie-slim
RUN apt -y update && apt -y upgrade \
    && apt -y --no-install-recommends install ca-certificates x11-apps make \
       gcc g++ cmake git qt6-base-dev libqt6charts6-dev qt6-translations-l10n \
       linguist-qt6 qt6-l10n-tools qt6-tools-\* qt6-image-formats-plugins \
       libreadline-dev qt6-declarative-dev pkg-config \
    && git clone https://github.com/travisgoodspeed/maskromtool.git \
    && cd maskromtool \
    && git submodule init \
    && git submodule update --remote \
    && mkdir build && cd build \
    && cmake .. \
    && make -j 8 \
    && make install
ARG GROUP_ID
ARG USER_ID
ARG USER_NAME
RUN groupadd -g ${GROUP_ID} ${USER_NAME} \
    && useradd -g ${USER_NAME} -u ${USER_ID} \
       --shell /bin/bash --create-home ${USER_NAME}
USER ${USER_ID}
WORKDIR /home/${USER_NAME}
CMD ["/usr/local/bin/maskromtool"]
