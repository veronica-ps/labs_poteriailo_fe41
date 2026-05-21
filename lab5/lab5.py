import streamlit as st
import pandas as pd
import os
import plotly.express as px

st.set_page_config(page_title="Lab 5", layout="wide")


@st.cache_data
def load_and_clean_data():
    folder = "vhi_data"
    if not os.path.exists(folder):
        st.error(f"Папку '{folder}' не знайдено.")
        return pd.DataFrame()

    cols = ['Year', 'Week', 'SMN', 'SMT', 'VCI', 'TCI', 'VHI', 'empty']
    df_list = []

    for filename in os.listdir(folder):
        if filename.endswith(".csv"):
            pid = int(filename.split('_')[2])
            path = os.path.join(folder, filename)
            df = pd.read_csv(path, header=1, names=cols, skip_blank_lines=True)
            if 'empty' in df.columns:
                df = df.drop(columns=['empty'])
            df = df.dropna(subset=['VHI'])
            df = df[df['VHI'] != -1]
            df['Year'] = df['Year'].astype(str).str.replace(r'<[^>]*>', '', regex=True)
            df = df[df['Year'].str.strip() != '']
            df['Year'] = df['Year'].astype(int)
            df['Area_ID'] = pid
            df_list.append(df)

    if not df_list:
        return pd.DataFrame()

    data = pd.concat(df_list, ignore_index=True)

    index_dict = {1: 22, 2: 24, 3: 23, 4: 25, 5: 3, 6: 4, 7: 8, 8: 19, 9: 20, 10: 21,
                  11: 9, 12: 26, 13: 10, 14: 11, 15: 12, 16: 13, 17: 14, 18: 15, 19: 16,
                  20: 27, 21: 17, 22: 18, 23: 6, 24: 1, 25: 2, 26: 7, 27: 5}
    names_dict = {1: 'Вінницька', 2: 'Волинська', 3: 'Дніпропетровська', 4: 'Донецька',
                  5: 'Житомирська', 6: 'Закарпатська', 7: 'Запорізька', 8: 'Івано-Франківська',
                  9: 'Київська', 10: 'Кіровоградська', 11: 'Луганська', 12: 'Львівська',
                  13: 'Миколаївська', 14: 'Одеська', 15: 'Полтавська', 16: 'Рівненська',
                  17: 'Сумська', 18: 'Тернопільська', 19: 'Харківська', 20: 'Херсонська',
                  21: 'Хмельницька', 22: 'Черкаська', 23: 'Чернівецька', 24: 'Чернігівська',
                  25: 'Республіка Крим', 26: 'м. Київ', 27: 'м. Севастополь'}

    data['Area_ID'] = data['Area_ID'].map(index_dict)
    data['Area_Name'] = data['Area_ID'].map(names_dict)
    return data


df = load_and_clean_data()

if not df.empty:
    st.sidebar.header("Параметри фільтрації")

  
    if st.sidebar.button("Скинути всі фільтри"):
        st.session_state.clear()
        st.rerun()
      
    index_choice = st.sidebar.selectbox("Виберіть індекс:", ["VCI", "TCI", "VHI"], key="idx")
    area_choice = st.sidebar.selectbox("Виберіть область:", df['Area_Name'].dropna().unique(), key="area")

    min_year, max_year = int(df['Year'].min()), int(df['Year'].max())
    year_range = st.sidebar.slider("Інтервал років:", min_year, max_year, (min_year, max_year), key="years")
    week_range = st.sidebar.slider("Інтервал тижнів:", 1, 52, (1, 52), key="weeks")

    st.sidebar.subheader("Сортування")
    sort_asc = st.sidebar.checkbox("За зростанням", key="asc")
    sort_desc = st.sidebar.checkbox("За спаданням", key="desc")

    filtered_df = df[
        (df['Area_Name'] == area_choice) &
        (df['Year'] >= year_range[0]) & (df['Year'] <= year_range[1]) &
        (df['Week'] >= week_range[0]) & (df['Week'] <= week_range[1])
        ].copy()

    if sort_asc and sort_desc:
        st.sidebar.warning("Увімкнено обидва сортування, дані не відсортовано")
    elif sort_asc:
        filtered_df = filtered_df.sort_values(by=index_choice, ascending=True)
    elif sort_desc:
        filtered_df = filtered_df.sort_values(by=index_choice, ascending=False)

    st.title("Аналіз вегетаційних індексів України")
    tab1, tab2, tab3 = st.tabs(["Таблиця даних", "Графік області", "Порівняння областей"])

    with tab1:
        st.subheader(f"Дані для області: {area_choice}")
        st.dataframe(filtered_df[['Year', 'Week', 'Area_Name', 'SMN', 'SMT', 'VCI', 'TCI', 'VHI']],
                     use_container_width=True)

    with tab2:
        st.subheader(f"Динаміка {index_choice} для {area_choice}")
        if not filtered_df.empty:
            plot_df = filtered_df.sort_values(by=['Year', 'Week'])
            plot_df['Time'] = plot_df['Year'].astype(str) + " - W" + plot_df['Week'].astype(str)
            fig1 = px.line(plot_df, x='Time', y=index_choice, title=f"Часовий ряд {index_choice}")
            st.plotly_chart(fig1, use_container_width=True)
        else:
            st.warning("Немає даних для відображення графіка.")

    with tab3:
        st.subheader(f"Порівняння {index_choice} з іншими областями")
        compare_df = df[
            (df['Year'] >= year_range[0]) & (df['Year'] <= year_range[1]) &
            (df['Week'] >= week_range[0]) & (df['Week'] <= week_range[1])
            ]
        if not compare_df.empty:
            avg_compare = compare_df.groupby('Area_Name')[index_choice].mean().reset_index()
            avg_compare['Колір'] = avg_compare['Area_Name'].apply(
                lambda x: 'Обрана область' if x == area_choice else 'Інші області')

            fig2 = px.bar(avg_compare, x='Area_Name', y=index_choice, color='Колір',
                          title=f"Середнє значення {index_choice} по областях",
                          color_discrete_map={'Обрана область': 'red', 'Інші області': 'blue'})
            fig2.update_layout(xaxis_title="Область", yaxis_title=f"Середній {index_choice}",
                               xaxis={'categoryorder': 'total descending'})
            st.plotly_chart(fig2, use_container_width=True)
        else:
            st.warning("Немає даних для порівняння")
