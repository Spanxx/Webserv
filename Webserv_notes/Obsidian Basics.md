📆 2025-04-25 🕓 10:50
Tags: #obsidian #wiki

---

```ad-hint
You can click each line and see the "special" chars which are used to create the formating.
```

# Markdown Syntax

- adding a # in front of text **without** a space creates a tag
- adding a # in front of text **with** a space creates a heading

1 # creates a h1 heading
2 # creates a h2 heading (smaller)
...
# h1
## h2
### h3

2 * signs in front and end of text **makes it bold**

`- [ ] `  creates a checkbox (minus space squareb open space squareb close space)

- [ ] learned something about Obsidian today :)

\` Sign creates a single code line, it creates 2 and you can write directly in it
`This is a code line`

\`\`\` Three create a code block
Creating a code block and writing the coding language behind add the language icon
You can also add a title with `title:"This is a title"`

```cpp title:"Learning about codeblocks"
```

```js title:"puuh...webdev will be hard"
```


Writing 2 sqaure brackets around text is creating a link to a file or another note.
When the note doesn't exist, it creates it.
Good for linking notes if they have matching content.

[[General Overview]]

Creating a link to a picture (e.g from excalidraw) you can add a ! in front, that it is shown in the note.

![[Hello World!]]
# Using templates

With the shortcut `ALT + T` you can use the simple template i created.

It is called std_notes.
It adds the current date and time, a line for tags to connect topics with each other and a separation line.

# Image scaling

Add pipe `|` behinde the filename inside the square brackets for the width, obsidian will scale the height automatically.
- `![[image.png|width]]` scales to width pixels and automatically scales the heigt
- `![[image.png|widthxheight]]` scales to width x height pixels

# Shortcuts:

- Toogle spellcheck = ctrl + shift + /
- Insert codeblock = ctrl + alt + c
- Using Template = alt + t

# Admonition
Create a codeblock and ad `ad-<type>` in first line.

\`\`\` ad-TYPE
\`\`\`
## Parameters 

```
"```ad- # Admonition type. See below for a list of available types.  
title: # Admonition title.  
collapse: # Create a collapsible admonition.  
icon: # Override the icon.  
color: # Override the color.
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla et euismod nulla.
```"
```

```ad-example
This is an example
```


# Git in Obsidian

There is a little Graph sign on the left side or on the top right.

It opens the git graph overview and you can upload your notes to our Git repo.
Stage all and click "Commit and sync" and then "Push".
